package com.sunfusheng.ffmpeg.jupiter;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import com.qw.soul.permission.SoulPermission;
import com.qw.soul.permission.bean.Permission;
import com.qw.soul.permission.bean.Permissions;
import com.qw.soul.permission.callbcak.CheckRequestPermissionsListener;
import com.sunfusheng.ffmpeg.jupiter.ffmpeg.FFmpegWrapper;
import java.io.File;

public class MainActivity extends AppCompatActivity {

  private static final String TAG = "MainActivity";
  private static final int REQUEST_CODE_PICK_VIDEO_FILE = 100;

  private String path1 = Environment.getExternalStorageDirectory().getAbsolutePath() +
      File.separator + "zhongyanggongyuan.mp4";
  private String path2 = Environment.getExternalStorageDirectory().getAbsolutePath() +
      File.separator + "biaoqingmofang.mp4";

  private TextView vText;
  private ImageView vImage;

  private StringBuilder mText = new StringBuilder();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    vText = findViewById(R.id.text);
    vImage = findViewById(R.id.image);

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
      mText.append("CPU Supported ABIS: ");
      for (int i = 0; i < Build.SUPPORTED_ABIS.length; i++) {
        mText.append("\n").append(i + 1).append("、").append(Build.SUPPORTED_ABIS[i]);
      }
    }
    mText.append("\n\nFFmpeg Version: ").append(FFmpegWrapper.getFFmpegVersion());
    vText.setText(mText);

    SoulPermission.getInstance().checkAndRequestPermissions(Permissions.build(
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.WRITE_EXTERNAL_STORAGE), new CheckRequestPermissionsListener() {
      @Override
      public void onAllPermissionOk(Permission[] allPermissions) {
        initFFmpeg();
      }

      @Override
      public void onPermissionDenied(Permission[] refusedPermissions) {

      }
    });
  }

  private void initFFmpeg() {
    long startTime = System.currentTimeMillis();
    Bitmap bitmap = FFmpegWrapper.getVideoFirstFrame(path1);
    Log.d(TAG, "Time consumed: " + (System.currentTimeMillis() - startTime) + "ms");
    if (bitmap != null) {
      vImage.setImageBitmap(bitmap);
    }
  }

  private void pickVideoFile() {
    Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
    intent.addCategory(Intent.CATEGORY_OPENABLE);
    intent.setType("*/*");
    String[] mimeTypes = { "video/*" };
    intent.putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes);
    startActivityForResult(intent, REQUEST_CODE_PICK_VIDEO_FILE);
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
    if (requestCode == REQUEST_CODE_PICK_VIDEO_FILE && resultCode == Activity.RESULT_OK) {
      if (data != null) {
        Log.d(TAG, "[sfs] uri: " + data.getData());
      }
    }
  }
}