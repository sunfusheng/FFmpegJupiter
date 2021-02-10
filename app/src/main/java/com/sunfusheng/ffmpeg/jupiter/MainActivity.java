package com.sunfusheng.ffmpeg.jupiter;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import com.sunfusheng.ffmpeg.jupiter.ffmpeg.FFmpegUtils;

public class MainActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    TextView tv = findViewById(R.id.sample_text);
    tv.setText("FFmpeg Version: " + FFmpegUtils.getFFmpegVersion());
  }
}