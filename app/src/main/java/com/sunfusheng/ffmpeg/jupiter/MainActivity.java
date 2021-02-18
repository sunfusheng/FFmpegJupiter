package com.sunfusheng.ffmpeg.jupiter;

import android.os.Bundle;
import android.os.Environment;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import com.sunfusheng.ffmpeg.jupiter.ffmpeg.FFmpegUtils;
import java.io.File;

public class MainActivity extends AppCompatActivity {

  final String path1 = Environment.getExternalStorageDirectory().getAbsolutePath() +
      File.separator + "zhongyanggongyuan.mp4";
  final String path2 = Environment.getExternalStorageDirectory().getAbsolutePath() +
      File.separator + "biaoqingmofang.mp4";

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    TextView tv = findViewById(R.id.sample_text);
    tv.setText("FFmpeg Version: " + FFmpegUtils.getFFmpegVersion());

    FFmpegUtils.getVideoFirstFrame(path1);
  }
}