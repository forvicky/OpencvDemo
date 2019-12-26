#include <jni.h>
#include <string>
#include <vector>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/hal/interface.h>
#include "bitmap_utils.h"
#include "FaceDector.h"
#include "TxtDector.h"

using namespace std;



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

    detectTextRegion(srcMat);
//    dstMat=preprocess(srcMat);
    return createBitmap(env, srcMat, argb8888);//使用dstMat创建一个Bitmap对象
}




extern "C"
JNIEXPORT jint JNICALL
Java_com_zdd_opencvdemo_MainActivity_faceDetector(JNIEnv *env, jclass type, jobject bitmap,
                                                  jobject argb8888, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    FaceDetector::loadCascade(path);//加载文件

    Mat srcMat;//图片源矩阵
    bitmap2Mat(env, bitmap, &srcMat);//图片源矩阵初始化
    auto faces = FaceDetector::detectorFace(srcMat);//识别图片源矩阵，返回矩形集

    for (Rect faceRect : faces) {// 在人脸部分画矩形
        rectangle(srcMat, faceRect, Scalar(0, 253, 255), 5);//在srcMat上画矩形
        mat2Bitmap(env, srcMat, bitmap);// 把mat放回bitmap中
    }

    env->ReleaseStringUTFChars(path_, path);

    return faces.size();
}