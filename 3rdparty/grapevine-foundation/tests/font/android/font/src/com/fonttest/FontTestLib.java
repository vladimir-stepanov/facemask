
package com.fonttest;

import android.content.res.AssetManager;
import android.util.Log;

// Wrapper for native library

public class FontTestLib
{
     static {
         System.loadLibrary("grapevine_png");
         System.loadLibrary("gnustl_shared");
         System.loadLibrary("sigcpp");
         System.loadLibrary("iconv");
         System.loadLibrary("bullet");
         System.loadLibrary("grapevine");
         System.loadLibrary("fonttest");
     }

    public static void init()
    {
        Log.i("FontTestLib", String.format("init\n"));
    }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(int width, int height);
     public static native void step();
     public static native void touch(float x, float y, float dx, float dy, int flags);
	 public static native void setAssetManager(AssetManager assetManager);
}
