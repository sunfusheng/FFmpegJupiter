package com.sunfusheng.ffmpeg.jupiter.ffmpeg;

/**
 * @author sunfusheng
 * @since 2021/02/09
 */
public class FFmpegUtils {

  static {
    System.loadLibrary("avcodec");
    System.loadLibrary("avdevice");
    System.loadLibrary("avfilter");
    System.loadLibrary("avformat");
    System.loadLibrary("avutil");
    System.loadLibrary("swresample");
    System.loadLibrary("swscale");
    System.loadLibrary("ffmpeg-jupiter");
  }

  public static native String getFFmpegVersion();
}
