//
// Created by yujy05 on 2019/10/22.
//

#ifndef QRFINDER_CV_HELPER_H
#define QRFINDER_CV_HELPER_H


#include <jni.h>
#include "opencv2/opencv.hpp"
using namespace cv;

class cv_helper {
public:
    //transform bitmap To mat
    static void bitmapToMat(JNIEnv *env, jobject& bitmap, Mat& mat);

    static void bitmapToMat(JNIEnv *env, jobject& bitmap, Mat& mat, jboolean needUnPreMultiplyAlpha);

    //transform mat to bitmap
    static void matTobitmap(JNIEnv *env, jobject& bitmap, Mat& mat,jboolean needPreMultiplyAlpha);

};


#endif //QRFINDER_CV_HELPER_H
