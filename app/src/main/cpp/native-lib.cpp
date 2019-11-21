#include <jni.h>
#include <string>
#include <vector>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/hal/interface.h>
#include "bitmap_utils.h"

using namespace std;

Mat preprocess(Mat gray)
{
    //1.Sobel算子，x方向求梯度
    Mat sobel;
    Sobel(gray, sobel, CV_8U, 1, 0, 3);

    //2.二值化
    Mat binary;
    threshold(sobel, binary, 0, 255, THRESH_OTSU + THRESH_BINARY);

    //3.膨胀和腐蚀操作核设定
    Mat element1 = getStructuringElement(MORPH_RECT, Size(30, 9));
    //控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
    Mat element2 = getStructuringElement(MORPH_RECT, Size(24, 4));

    //4.膨胀一次，让轮廓突出
    Mat dilate1;
    dilate(binary, dilate1, element2);

    //5.腐蚀一次，去掉细节，表格线等。这里去掉的是竖直的线
    Mat erode1;
    erode(dilate1, erode1, element1);

    //6.再次膨胀，让轮廓明显一些
    Mat dilate2;
    dilate(erode1, dilate2, element2);

    //7.存储中间图片
    imwrite("binary.jpg", binary);
    imwrite("dilate1.jpg", dilate1);
    imwrite("erode1.jpg", erode1);
    imwrite("dilate2.jpg", dilate2);

    return dilate2;
}


vector<RotatedRect> findTextRegion(Mat img)
{
    vector<RotatedRect> rects;
    //1.查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));

    //2.筛选那些面积小的
    for (int i = 0; i < contours.size(); i++)
    {
        //计算当前轮廓的面积
        double area = contourArea(contours[i]);

        //面积小于1000的全部筛选掉
        if (area < 1000)
            continue;

        //轮廓近似，作用较小，approxPolyDP函数有待研究
        double epsilon = 0.001*arcLength(contours[i], true);
        Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //找到最小矩形，该矩形可能有方向
        RotatedRect rect = minAreaRect(contours[i]);

        //计算高和宽
        int m_width = rect.boundingRect().width;
        int m_height = rect.boundingRect().height;

        //筛选那些太细的矩形，留下扁的
        if (m_height > m_width * 1.2)
            continue;

        //符合条件的rect添加到rects集合中
        rects.push_back(rect);

    }
    return rects;
}

Mat detect(Mat img)
{
    //1.转化成灰度图
    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);

    //2.形态学变换的预处理，得到可以查找矩形的轮廓
    Mat dilation = preprocess(gray);

    //3.查找和筛选文字区域
    vector<RotatedRect> rects = findTextRegion(dilation);

    //4.用绿线画出这些找到的轮廓
    for(int i=0;i<rects.size();i++){
        RotatedRect rect=rects[i];

        Point2f P[4];
        rect.points(P);
        for (int j = 0; j <= 3; j++)
        {
            line(img, P[j], P[(j + 1) % 4], Scalar(0,255,0), 2, 8, 0);
        }

    }

    return img;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_zdd_opencvdemo_MainActivity_cvtBitmap(JNIEnv *env, jobject instance, jobject bitmap,
                                              jobject argb8888) {
    Mat srcMat;
    Mat dstMat;
    bitmap2Mat(env, bitmap, &srcMat);
    cvtColor(srcMat, dstMat, CV_BGR2GRAY);//将图片的像素信息灰度化盛放在dstMat
    return createBitmap(env, dstMat, argb8888);//使用dstMat创建一个Bitmap对象
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_zdd_opencvdemo_MainActivity_matchBitmap(JNIEnv *env, jobject instance, jobject bigBitmap,
                                           jobject smallBitmap,
                                           jobject argb8888, int match_method) {
    Mat bigMat;
    Mat smallMat;
    bitmap2Mat(env, bigBitmap, &bigMat);
    bitmap2Mat(env, smallBitmap, &smallMat);

    // / Create the result matrix
    int result_cols = bigMat.cols - smallMat.cols + 1;
    int result_rows = bigMat.rows - smallMat.rows + 1;

    Mat resultMat;
    resultMat.create(result_rows, result_cols,CV_32FC1);

    //TM_CCOEFF_NORMED
    matchTemplate(bigMat, smallMat, resultMat, match_method);
    //normalize查找全局最小和最大稀疏数组元素并返回其值及其位置
    normalize(resultMat, resultMat, 0, 1, NORM_MINMAX, -1);

    double minValue;
    double maxValue;
    Point minLocation;
    Point maxLocation;
    Point matchLocation;

    minMaxLoc(resultMat, &minValue, &maxValue, &minLocation, &maxLocation, Mat());

    //【5】对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值有着更高的匹配结果. 而其余的方法, 数值越大匹配效果越好

    if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
        matchLocation = minLocation;
    } else {
        matchLocation = maxLocation;
    }

    //【6】绘制出矩形，并显示最终结果
    rectangle(bigMat, matchLocation, Point(matchLocation.x + smallMat.cols, matchLocation.y + smallMat.rows), Scalar(0, 0, 255), 2, 8, 0);
//    rectangle(resultMat, matchLocation, Point(matchLocation.x + smallMat.cols, matchLocation.y + smallMat.rows), Scalar(0, 0, 255), 2, 8, 0);

    return createBitmap(env, bigMat, argb8888);//使用dstMat创建一个Bitmap对象
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_zdd_opencvdemo_MainActivity_txtAreaBitmap(JNIEnv *env, jobject instance, jobject bitmap,
                                               jobject argb8888) {
    Mat srcMat;
    Mat dstMat;

    bitmap2Mat(env, bitmap, &srcMat);

    detect(srcMat);
//    dstMat=preprocess(srcMat);
    return createBitmap(env, srcMat, argb8888);//使用dstMat创建一个Bitmap对象
}




