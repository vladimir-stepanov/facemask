package com.hfs.furyclient.opencv;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.util.Arrays;
import java.util.List;

/**
 * Created by EStepanova on 06.12.2017.
 */

public class HFSObjTracker {
    private static final String TAG = "HFSObjTracker";
    private static final Object LOCK = new Object();
    public static final String MEDIANFLOW_TRACKER_ALGORITHM = "MEDIANFLOW";
    private static final String CASCADE = "Facemask/haarcascade_fullbody.xml";

    @SuppressLint("StaticFieldLeak")
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

    private boolean mInitiated;
    private boolean mInitialising;

    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeObjTrackerContext;
    @SuppressWarnings("unused")
    private long mNativeObjDetectorContext;
    public long mSpentTime;

    private HFSObjTracker() {
        mInitiated = false;
        mInitialising = false;
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

    public boolean isInitiated() {
        synchronized (LOCK) {
            return mInitiated;
        }
    }

    @SuppressWarnings("SameParameterValue")
    private void setInitiated(boolean initiated) {
        synchronized (LOCK) {
            mInitiated = initiated;
        }
    }

    private boolean isInitialising() {
        synchronized (LOCK) {
            return mInitialising;
        }
    }

    private void setInitialising(boolean initialising) {
        synchronized (LOCK) {
            mInitialising = initialising;
        }
    }

    private void init() {
        if (isInitialising()) {
            return;
        }
        setInitialising(true);
        jniInit(new File(Environment.getExternalStorageDirectory(), CASCADE).getAbsolutePath());
        setInitiated(true);
        setInitialising(false);
    }

    public void asyncInit() {
        new HFSObjTracker.InitThread().start();
    }

    public List<Rect> detect(Bitmap bitmap, String alg) {
        if (isInitiated()) {
            Rect[] objs = jniDetect(bitmap, alg);
            return Arrays.asList(objs);
        } else {
            return null;
        }
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
    private synchronized native int jniInit(String cascadeClassifierPath);

    private synchronized native Rect[] jniDetect(Bitmap bitmap, String alg);

    private class InitThread extends Thread {

        InitThread() {
            super("InitThread");
        }

        public void run() {
            init();
        }

    }
}
