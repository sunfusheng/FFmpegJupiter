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

static const char *FFmpegWrapperClassPath = "com/sunfusheng/ffmpeg/jupiter/FFmpegWrapper";
static const char *TAG = "[sfs] ffmpeg_wrapper";

#define logDebug(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define logWarn(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define logError(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

static jstring get_ffmpeg_version(JNIEnv *env, jclass clazz) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

// 创建一个ARGB_8888的Bitmap，通过Bitmap.createBitmap(width. height, Bitmap.Config.valueOf("ARGB_8888"))
static jobject createBitmap(JNIEnv *env, int width, int height) {
    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMethod = env->GetStaticMethodID(bitmapClass,
                                                          "createBitmap",
                                                          "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID bitmapConfigValueOfMethod = env->GetStaticMethodID(bitmapConfigClass,
                                                                 "valueOf",
                                                                 "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jstring ARGB_8888 = env->NewStringUTF("ARGB_8888");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       bitmapConfigValueOfMethod,
                                                       ARGB_8888);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapClass,
                                                    createBitmapMethod,
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

    AVCodecParameters *codecpar = nullptr;
    int video_stream_index = -1; // 视频流索引位置
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

    // 创建Bitmap对象
    jobject bitmap = createBitmap(env, codecpar->width, codecpar->height);
    void *bitmap_ptr;
    ret = AndroidBitmap_lockPixels(env, bitmap, &bitmap_ptr);
    if (ret < 0) {
        logError("AndroidBitmap_lockPixels error");
        return nullptr;
    }

    // 申请帧结构体
    AVFrame *av_frame = av_frame_alloc();
    if (!av_frame) {
        logError("申请帧结构体失败");
        return nullptr;
    }

    int av_frame_ptr;
    AVPacket av_pkt;
    while (av_read_frame(fmt_ctx, &av_pkt) >= 0) {
        if (av_pkt.stream_index != video_stream_index) {
            continue;
        }

        avcodec_decode_video2(codec_ctx, av_frame, &av_frame_ptr, &av_pkt);
        av_packet_unref(&av_pkt);
        if (av_frame_ptr) {
            logDebug("首帧读取完毕");
            break;
        }
    }

    // 将YUV转换为ARGB
    int line_size;
    line_size = av_frame->width * 4;
    libyuv::I420ToABGR(av_frame->data[0], av_frame->linesize[0],
                       av_frame->data[1], av_frame->linesize[1],
                       av_frame->data[2], av_frame->linesize[2],
                       (uint8_t *) bitmap_ptr, line_size,
                       av_frame->width, av_frame->height);

    // 释放资源

    AndroidBitmap_unlockPixels(env, bitmap);
    av_free(av_frame);
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
    jclass clazz = env->FindClass(FFmpegWrapperClassPath);
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