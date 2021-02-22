package com.sunfusheng.ffmpeg.jupiter

import android.Manifest.permission
import android.content.Intent
import android.os.Build
import android.os.Build.VERSION
import android.os.Build.VERSION_CODES
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import androidx.appcompat.app.AppCompatActivity
import com.qw.soul.permission.SoulPermission
import com.qw.soul.permission.bean.Permission
import com.qw.soul.permission.bean.Permissions
import com.qw.soul.permission.callbcak.CheckRequestPermissionsListener
import com.sunfusheng.ffmpeg.jupiter.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {

    companion object {
        private const val TAG = "MainActivity"
        private const val REQUEST_CODE_PICK_VIDEO_FILE = 100
    }

    private val path1 = Environment.getExternalStorageDirectory().absolutePath +
            File.separator + "zhongyanggongyuan.mp4"
    private val path2 = Environment.getExternalStorageDirectory().absolutePath +
            File.separator + "biaoqingmofang.mp4"

    private lateinit var binding: ActivityMainBinding

    private val mText = StringBuilder()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(LayoutInflater.from(this))
        setContentView(binding.root)

        if (VERSION.SDK_INT >= VERSION_CODES.LOLLIPOP) {
            mText.append("CPU Supported ABI: ").append(Build.SUPPORTED_ABIS[0]);
        }
        mText.append("\nFFmpeg Version: ").append(FFmpegWrapper.getFFmpegVersion())
        binding.vText.text = mText

        SoulPermission.getInstance().checkAndRequestPermissions(
                Permissions.build(permission.READ_EXTERNAL_STORAGE, permission.WRITE_EXTERNAL_STORAGE),
                object : CheckRequestPermissionsListener {
                    override fun onAllPermissionOk(allPermissions: Array<Permission>) {
                        initFFmpeg()
                    }

                    override fun onPermissionDenied(refusedPermissions: Array<Permission>) {}
                })
    }

    private fun initFFmpeg() {
        val startTime = System.currentTimeMillis()
        val bitmap = FFmpegWrapper.getVideoFirstFrame(path1)
        Log.d(TAG, "Time consumed: " + (System.currentTimeMillis() - startTime) + "ms")
        binding.vImage.setImageBitmap(bitmap)
    }

    private fun pickVideoFile() {
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT)
        intent.addCategory(Intent.CATEGORY_OPENABLE)
        intent.type = "*/*"
        val mimeTypes = arrayOf("video/*")
        intent.putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes)
        startActivityForResult(intent, REQUEST_CODE_PICK_VIDEO_FILE)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == REQUEST_CODE_PICK_VIDEO_FILE && resultCode == RESULT_OK) {
            if (data != null) {
                Log.d(TAG, "[sfs] uri: " + data.data)
            }
        }
    }
}