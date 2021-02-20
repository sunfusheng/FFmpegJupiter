package com.sunfusheng.ffmpeg.jupiter

import android.graphics.Bitmap

/**
 * @author sunfusheng
 * @since 2021/02/09
 */
object FFmpegWrapper {

  init {
    System.loadLibrary("avcodec")
    System.loadLibrary("avdevice")
    System.loadLibrary("avfilter")
    System.loadLibrary("avformat")
    System.loadLibrary("avutil")
    System.loadLibrary("swresample")
    System.loadLibrary("swscale")
    System.loadLibrary("yuv")
    System.loadLibrary("ffmpeg-jupiter")
  }

  external fun getFFmpegVersion(): String?

  external fun getVideoFirstFrame(path: String?): Bitmap?
}