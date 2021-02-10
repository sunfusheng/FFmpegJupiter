//
// Created by sunfusheng on 2/9/21.
//
#include <jni.h>
#include <string>
#include <cstdio>
#include "android/log.h"

extern "C" {
#include "include/libavutil/avutil.h"

static const char *FFmpegUtilsClassName = "com/sunfusheng/ffmpeg/jupiter/ffmpeg/FFmpegUtils";

#define logDebug(...) __android_log_print(ANDROID_LOG_DEBUG,"[sfs] ffmpeg_utils",__VA_ARGS__)
#define logWarn(...) __android_log_print(ANDROID_LOG_WARN,"[sfs] ffmpeg_utils",__VA_ARGS__)
#define logError(...) __android_log_print(ANDROID_LOG_ERROR,"[sfs] ffmpeg_utils",__VA_ARGS__)

jstring getFFmpegVersion(JNIEnv *env, jclass clazz) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

static const JNINativeMethod gMethods[] = {
        {"getFFmpegVersion", "()Ljava/lang/String;", (void *) getFFmpegVersion}
};

int registerNatives(JNIEnv *env) {
    logDebug("registerNatives()");
    jclass clazz = env->FindClass(FFmpegUtilsClassName);
    if (clazz == nullptr) {
        return JNI_ERR;
    }
    int methods_count = sizeof(gMethods) / sizeof(JNINativeMethod);
    if (env->RegisterNatives(clazz, gMethods, methods_count) != JNI_OK) {
        return JNI_ERR;
    }
    return JNI_OK;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    logDebug("JNI_OnLoad()");
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    if (registerNatives(env) != JNI_OK) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

}