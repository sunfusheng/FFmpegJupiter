//
// Created by sunfusheng on 2/9/21.
//
#include <jni.h>
#include <string>
#include <cstdio>
#include "android/log.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"

static const char *FFmpegUtilsClassName = "com/sunfusheng/ffmpeg/jupiter/ffmpeg/FFmpegUtils";

#define logDebug(...) __android_log_print(ANDROID_LOG_DEBUG,"[sfs] ffmpeg_utils",__VA_ARGS__)
#define logWarn(...) __android_log_print(ANDROID_LOG_WARN,"[sfs] ffmpeg_utils",__VA_ARGS__)
#define logError(...) __android_log_print(ANDROID_LOG_ERROR,"[sfs] ffmpeg_utils",__VA_ARGS__)

static jstring get_ffmpeg_version(JNIEnv *env, jclass clazz) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

static int get_video_first_frame(JNIEnv *env, jclass clazz, jstring path) {
    const char *url = env->GetStringUTFChars(path, JNI_FALSE);
    logDebug("视频路径: %s", url);

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    int ret = avformat_open_input(&fmt_ctx, url, nullptr, nullptr);
    if (ret < 0) {
        logError("解封装失败: %s", av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        logError("查找流失败: %s", av_err2str(ret));
        return -1;
    }

    int video_stream_index = -1; // 视频流索引位置
    AVCodecParameters *codecpar = nullptr;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            codecpar = stream->codecpar;
            break;
        }
    }

    if (video_stream_index == -1) {
        logError("找不到视频流");
        return -1;
    }

    logDebug("视频流索引位置：%d", video_stream_index);
    logDebug("视频分辨率：%dx%d", codecpar->width, codecpar->height);
    logDebug("视频解码器: %s", avcodec_get_name(codecpar->codec_id));

    // 找到最合适的流
    av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    avformat_close_input(&fmt_ctx);
    return 0;
}


static const JNINativeMethod gMethods[] = {
        {"getFFmpegVersion",   "()Ljava/lang/String;",  (void *) get_ffmpeg_version},
        {"getVideoFirstFrame", "(Ljava/lang/String;)I", (void *) get_video_first_frame}
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