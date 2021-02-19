package com.sunfusheng.ffmpeg.jupiter;

import android.graphics.Bitmap;

/**
 * @author sunfusheng
 * @since 2021/02/09
 */
public class FFmpegWrapper {

  static {
    System.loadLibrary("avcodec");
    System.loadLibrary("avdevice");
    System.loadLibrary("avfilter");
    System.loadLibrary("avformat");
    System.loadLibrary("avutil");
    System.loadLibrary("swresample");
    System.loadLibrary("swscale");
    System.loadLibrary("yuv");
    System.loadLibrary("ffmpeg-jupiter");
  }

  public static native String getFFmpegVersion();

  public static native Bitmap getVideoFirstFrame(String path);
}
