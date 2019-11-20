#include <jni.h>
#include <string>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/hal/interface.h>
#include "bitmap_utils.h"

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
    rectangle(resultMat, matchLocation, Point(matchLocation.x + smallMat.cols, matchLocation.y + smallMat.rows), Scalar(0, 0, 255), 2, 8, 0);

    return createBitmap(env, bigMat, argb8888);//使用dstMat创建一个Bitmap对象
}