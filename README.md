# FFmpegJupiter

1、获取视频文件首帧封面图


Bitmap createBitmap(int width, int height, Config config)
创建一个空位图，里面的像素点没有色彩值。

int AndroidBitmap_lockPixels(JNIEnv* env, jobject jbitmap, void** addrPtr)
对图片进行解码并获取解码后像素保存在内存中的地址指针addrPtr，
通过对addrPtr指向的内存空间进行像素修改，就相当于直接修改了被加载到内存中的位图。

int AndroidBitmap_unlockPixels(JNIEnv* env, jobject jbitmap)
释放锁定 在内存中被修改的位图数据就可以用于显示到前台。