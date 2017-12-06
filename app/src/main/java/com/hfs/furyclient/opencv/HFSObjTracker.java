package com.hfs.furyclient.opencv;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.util.Log;

import java.util.Arrays;
import java.util.List;

public class HFSObjTracker {
    private static final String TAG = "HFSObjTracker";
    private static final Object LOCK = new Object();

    /**
     * Converts YUV420 to ARGB8888
     *
     * @param y
     * @param u
     * @param v
     * @param output
     * @param width
     * @param height
     * @param yRowStride
     * @param uvRowStride
     * @param uvPixelStride
     */
    public static native void convertYUV420ToARGB8888(
            byte[] y,
            byte[] u,
            byte[] v,
            int[] output,
            int width,
            int height,
            int yRowStride,
            int uvRowStride,
            int uvPixelStride);

    private static HFSObjTracker sObjTracker = null;

    static {
        try {
            System.loadLibrary("tracker_jni");
            jniNativeClassInit();
            Log.d(TAG, "jniNativeClassInit success");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "library not found");
        }
    }

    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeObjTrackerContext;

    private HFSObjTracker() {
    }

    public static HFSObjTracker getInstance() {
        synchronized (LOCK) {
            if (sObjTracker == null) {
                sObjTracker = new HFSObjTracker();
            }
            return sObjTracker;
        }
    }

    private native static void jniNativeClassInit();

    public void init(Bitmap bitmap, List<Rect> rectList) {
        int[] rectArray = new int[rectList.size() * 4];
        for (int i = 0; i < rectList.size(); i++) {
            Rect rect = rectList.get(i);
            rectArray[i * 4] = rect.left;
            rectArray[i * 4 + 1] = rect.top;
            rectArray[i * 4 + 2] = rect.width();
            rectArray[i * 4 + 3] = rect.height();
        }
        jniInit(bitmap, rectArray);
    }

    public List<Rect> detect(Bitmap bitmap) {
        Rect[] objs = jniDetect(bitmap);
        return Arrays.asList(objs);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    public void release() {
        jniDel();
    }


    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniDel();

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native void jniInit(Bitmap bitmap, int[] rectArray);

    private synchronized native Rect[] jniDetect(Bitmap bitmap);

}
