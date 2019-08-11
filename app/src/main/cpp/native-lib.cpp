#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include "testcv.h"
#include "facedetector.h"
#include "opticalflow.h"

#define LOG_TAG "System.out"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

//将Mat转换为bitmap
jobject mat2bitmap(JNIEnv *env, cv::Mat &src, bool needPremultiplyAlpha, jobject bitmap_config) {
    jclass java_bitmap_class = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetStaticMethodID(java_bitmap_class, "createBitmap",
                                           "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class,
                                                 mid, src.size().width, src.size().height,
                                                 bitmap_config);
    AndroidBitmapInfo info;
    void *pixels = 0;

    try {
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);

        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (src.type() == CV_8UC1) {
                cvtColor(src, tmp, cv::COLOR_GRAY2RGBA);
            } else if (src.type() == CV_8UC3) {
                cvtColor(src, tmp, cv::COLOR_RGB2BGRA);
            } else if (src.type() == CV_8UC4) {
                if (needPremultiplyAlpha) {
                    cvtColor(src, tmp, cv::COLOR_RGBA2mRGBA);
                } else {
                    src.copyTo(tmp);
                }
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if (src.type() == CV_8UC1) {
                cvtColor(src, tmp, cv::COLOR_GRAY2BGR565);
            } else if (src.type() == CV_8UC3) {
                cvtColor(src, tmp, cv::COLOR_RGB2BGR565);
            } else if (src.type() == CV_8UC4) {
                cvtColor(src, tmp, cv::COLOR_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch (cv::Exception e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return bitmap;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}



extern "C"
JNIEXPORT jobject JNICALL
Java_dem_vac_tesseractocr_VaccaeOpenCVJNI_getCameraframebitbmp(JNIEnv *env, jclass type,
                                                               jobject bmp, jstring text_) {
    const char *text = env->GetStringUTFChars(text_, 0);

    AndroidBitmapInfo bitmapInfo;
    void *pixelscolor;
    int ret;

    //获取图像信息，如果返回值小于0就是执行失败
    if ((ret = AndroidBitmap_getInfo(env, bmp, &bitmapInfo)) < 0) {
        LOGI("AndroidBitmap_getInfo failed! error-%d", ret);
        return NULL;
    }

    //判断图像类型是不是RGBA_8888类型
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGI("BitmapInfoFormat error");
        return NULL;
    }

    //获取图像像素值
    if ((ret = AndroidBitmap_lockPixels(env, bmp, &pixelscolor)) < 0) {
        LOGI("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }

    //获取ArrayList类引用
    jclass list_jcls = env->FindClass("java/util/ArrayList");
    if (list_jcls == NULL) {
        LOGI("ArrayList没找到相关类!");
        return 0;
    }

    //获取ArrayList构造函数id
    jmethodID list_init = env->GetMethodID(list_jcls, "<init>", "()V");
    //创建一个ArrayList对象
    jobject list_obj = env->NewObject(list_jcls, list_init, "");


    //获取ArrayList对象的add()的methodID
    jmethodID list_add = env->GetMethodID(list_jcls, "add", "(Ljava/lang/Object;)Z");

    //生成源图像
    cv::Mat src(bitmapInfo.height, bitmapInfo.width, CV_8UC4, pixelscolor);

    //图像处理
//    std::vector<cv::Mat> outdsts=testcv::getrectdetector(src);
//    std::vector<cv::Mat> outdsts = facedetector::detectorface(src);
    std::vector <cv::Mat> outdsts = opticalflow::dealOpticalFlow(src);

    //获取原图片的参数
    jclass java_bitmap_class = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetMethodID(java_bitmap_class, "getConfig",
                                     "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(bmp, mid);
    //将SRC转换为图片
    jobject _bitmap = mat2bitmap(env, src, false, bitmap_config);

    env->CallBooleanMethod(list_obj, list_add, _bitmap);

    //判断有截出的图像后加入到返回的List<Bitmap>列表中
    if(outdsts.size()>0) {
        for (int i = 0; i < outdsts.size(); i++) {
            jobject dstbmp = mat2bitmap(env, outdsts[i], false, bitmap_config);
            env->CallBooleanMethod(list_obj, list_add, dstbmp);
        }
    }

    AndroidBitmap_unlockPixels(env, bmp);


    return list_obj;
}

extern "C"
JNIEXPORT void JNICALL
Java_dem_vac_tesseractocr_VaccaeOpenCVJNI_loadcascade(JNIEnv *env, jclass type, jstring filepath_) {
    const char *filepath = env->GetStringUTFChars(filepath_, 0);

    // TODO
    facedetector::loadcascade(const_cast<char *>(filepath));

    env->ReleaseStringUTFChars(filepath_, filepath);
}