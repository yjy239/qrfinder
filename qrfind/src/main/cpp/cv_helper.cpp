//
// Created by yujy05 on 2019/10/22.
//

#include "cv_helper.h"
#include <android/bitmap.h>
#include <android/log.h>
#include "Android_Define.h"


void cv_helper::bitmapToMat(JNIEnv *env, jobject &bitmap, Mat &mat, jboolean needUnPreMultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &dst = mat;
    try {
        LOGD("nBitmapToMat");
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        dst.create(info.height, info.width, CV_8UC4);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGD("nBitmapToMat: RGBA_8888 -> CV_8UC4");
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (needUnPreMultiplyAlpha)
                cvtColor(tmp, dst, COLOR_mRGBA2RGBA);
            else
                tmp.copyTo(dst);
        } else {
        // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            LOGD("nBitmapToMat: RGB_565 -> CV_8UC4");
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, COLOR_BGR5652RGBA);
        }
            AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception &e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nBitmapToMat catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nBitmapToMat catched unknown exception (...)");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }

}

void cv_helper::bitmapToMat(JNIEnv *env, jobject &bitmap, Mat &mat) {
    cv_helper::bitmapToMat(env,bitmap,mat,false);
}

void cv_helper::matTobitmap(JNIEnv *env, jobject &bitmap, Mat &mat,jboolean needPreMultiplyAlpha) {
    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &src = mat;
    try {
        LOGD("nMatToBitmap");
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(src.dims == 2 && info.height == (uint32_t) src.rows && info.width == (uint32_t) src.cols);
        CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (src.type() == CV_8UC1) {
                LOGD("nMatToBitmap: CV_8UC1 -> RGBA_8888");
                cvtColor(src, tmp, COLOR_GRAY2RGBA);
            } else if (src.type() == CV_8UC3) {
                LOGD("nMatToBitmap: CV_8UC3 -> RGBA_8888");
                cvtColor(src, tmp, COLOR_RGB2RGBA);
            } else if (src.type() == CV_8UC4) {
                LOGD("nMatToBitmap: CV_8UC4 -> RGBA_8888");
                if (needPreMultiplyAlpha)
                    cvtColor(src, tmp, COLOR_RGBA2mRGBA);
                else
                    src.copyTo(tmp);
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if (src.type() == CV_8UC1) {
                LOGD("nMatToBitmap: CV_8UC1 -> RGB_565");
                cvtColor(src, tmp, COLOR_GRAY2BGR565);
            } else if (src.type() == CV_8UC3) {
                LOGD("nMatToBitmap: CV_8UC3 -> RGB_565");
                cvtColor(src, tmp, COLOR_RGB2BGR565);
            } else if (src.type() == CV_8UC4) {
                LOGD("nMatToBitmap: CV_8UC4 -> RGB_565");
                cvtColor(src, tmp, COLOR_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch (const cv::Exception &e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nMatToBitmap catched cv::Exception: %s", e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        LOGE("nMatToBitmap catched unknown exception (...)");
        return;
    }


}
