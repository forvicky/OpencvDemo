#include <jni.h>
#include <string>
#include <opencv2/imgproc/types_c.h>
#include "bitmap_utils.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_zdd_opencvdemo_MainActivity_opBitmap(JNIEnv *env, jobject instance, jobject bitmap,
                                            jobject argb8888) {
    Mat srcMat;
    Mat dstMat;
    bitmap2Mat(env, bitmap, &srcMat);
    cvtColor(srcMat, dstMat, CV_BGR2GRAY);//将图片的像素信息灰度化盛放在dstMat
    return createBitmap(env,dstMat,argb8888);//使用dstMat创建一个Bitmap对象
}