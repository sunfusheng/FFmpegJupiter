package com.sunfusheng.ffmpeg.jupiter;

import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import com.sunfusheng.ffmpeg.jupiter.ffmpeg.FFmpegWrapper;
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
    TextView tv = findViewById(R.id.text);

    StringBuilder sb = new StringBuilder();
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
      sb.append("CPU Supported ABIS: ");
      for (int i = 0; i < Build.SUPPORTED_ABIS.length; i++) {
        sb.append("\n").append(i + 1).append("、").append(Build.SUPPORTED_ABIS[i]);
      }
    }
    sb.append("\n\nFFmpeg Version: ").append(FFmpegWrapper.getFFmpegVersion());

    tv.setText(sb);

    FFmpegWrapper.getVideoFirstFrame(path1);
  }
}