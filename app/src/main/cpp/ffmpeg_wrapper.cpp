//
// Created by sunfusheng on 2/9/21.
//
#include <jni.h>
#include <string>
#include <cstdio>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "android/log.h"
#include "android/bitmap.h"
#include "libyuv.h"

static const char *FFmpegUtilsClassName = "com/sunfusheng/ffmpeg/jupiter/FFmpegWrapper";
static const char *TAG = "[sfs] ffmpeg_wrapper";

#define logDebug(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define logWarn(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define logError(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

static jstring get_ffmpeg_version(JNIEnv *env, jclass clazz) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

static jobject createBitmap(JNIEnv *env, int width, int height) {
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass,
                                                                   "valueOf",
                                                                   "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction,
                                                       configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    width,
                                                    height,
                                                    bitmapConfig);
    return newBitmap;
}

static jobject get_video_first_frame(JNIEnv *env, jclass clazz, jstring path) {
    const char *url = env->GetStringUTFChars(path, JNI_FALSE);
    logDebug("视频路径: %s", url);

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    int ret = avformat_open_input(&fmt_ctx, url, nullptr, nullptr);
    if (ret < 0) {
        logError("解封装失败: %s", av_err2str(ret));
        return nullptr;
    }

    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        logError("查找流失败: %s", av_err2str(ret));
        return nullptr;
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
        return nullptr;
    }

    logDebug("视频流索引位置：%d", video_stream_index);
    logDebug("视频分辨率：%dx%d", codecpar->width, codecpar->height);
    logDebug("视频编解码器: %s", avcodec_get_name(codecpar->codec_id));

    // 寻找最合适的视频流
    av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    // 寻找编解码器
    AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        logError("编解码器未找到");
        return nullptr;
    }

    // 申请编解码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        logError("申请编解码器上下文失败");
        return nullptr;
    }

    // 拷贝AVCodecParameters到AVCodecContext
    ret = avcodec_parameters_to_context(codec_ctx, codecpar);
    if (ret < 0) {
        logError("拷贝AVCodecParameters到AVCodecContext失败: %s", av_err2str(ret));
        return nullptr;
    }

    // 打开编解码器
    ret = avcodec_open2(codec_ctx, codec, nullptr);
    if (ret < 0) {
        logError("打开编解码器失败: %s", av_err2str(ret));
        return nullptr;
    }

    // 申请帧结构体
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        logError("申请帧结构体失败");
        return nullptr;
    }

    jobject bitmap = createBitmap(env, codecpar->width, codecpar->height);
    void *addrPtr;
    ret = AndroidBitmap_lockPixels(env, bitmap, &addrPtr);
    if (ret < 0) {
        logError("AndroidBitmap_lockPixels error");
        return nullptr;
    }

    AVPacket packet;
    int framePtr;
    while (av_read_frame(fmt_ctx, &packet) >= 0) {
        if (packet.stream_index != video_stream_index) {
            continue;
        }

        avcodec_decode_video2(codec_ctx, frame, &framePtr, &packet);
        av_packet_unref(&packet);
        if (framePtr) {
            logDebug("首帧读取完毕");
            break;
        }
    }

    int line_size = frame->width * 4;
    libyuv::I420ToABGR(frame->data[0], frame->linesize[0],
                       frame->data[1], frame->linesize[1],
                       frame->data[2], frame->linesize[2],
                       (uint8_t *) addrPtr, line_size,
                       frame->width, frame->height);

    AndroidBitmap_unlockPixels(env, bitmap);
    av_free(frame);
    avcodec_close(codec_ctx);
    avformat_close_input(&fmt_ctx);
    return bitmap;
}


static const JNINativeMethod gMethods[] = {
        {"getFFmpegVersion",   "()Ljava/lang/String;",                          (void *) get_ffmpeg_version},
        {"getVideoFirstFrame", "(Ljava/lang/String;)Landroid/graphics/Bitmap;", (void *) get_video_first_frame}
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