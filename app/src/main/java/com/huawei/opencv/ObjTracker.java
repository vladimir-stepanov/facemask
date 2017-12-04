package com.huawei.opencv;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import com.huawei.facemask.R;

import java.io.File;
import java.util.Arrays;
import java.util.List;

public class ObjTracker {
    private static final String TAG = "ObjTracker";
    protected static final Object LOCK = new Object();
    public static final String KCF_TRACKER_ALGORITHM = "KCF";
    public static final String MIL_TRACKER_ALGORITHM = "MIL";
    public static final String TLD_TRACKER_ALGORITHM = "TLD";
    public static final String MEDIANFLOW_TRACKER_ALGORITHM = "MEDIANFLOW";
    public static final String BOOSTING_TRACKER_ALGORITHM = "BOOSTING";
    private static final String CASCADE = "Facemask/lbpcascade_frontalface.xml";

    @SuppressLint("StaticFieldLeak")
    private static ObjTracker sObjTracker = null;

    static {
        try {
            System.loadLibrary("tracker_jni");
            jniNativeClassInit();
            Log.d(TAG, "jniNativeClassInit success");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "library not found");
        }
    }

    private Context mContext;
    private boolean mInitiated;
    private boolean mInitialising;

    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeFaceTrackerContext;
    @SuppressWarnings("unused")
    private long mNativeFaceDetectorContext;
    public long mSpentTime;

    private ObjTracker(Context context) {
        mInitiated = false;
        mInitialising = false;
        mContext = context;
    }

    public static ObjTracker getInstance(Context context) {
        synchronized (LOCK) {
            if (sObjTracker == null) {
                sObjTracker = new ObjTracker(context);
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

    public void init() {
        if (isInitialising()) {
            return;
        }
        setInitialising(true);
        jniInit(new File(Environment.getExternalStorageDirectory(), CASCADE).getAbsolutePath());
        setInitiated(true);
        setInitialising(false);
    }

    public void asyncInit() {
        new ObjTracker.InitThread(mContext).start();
    }

    public List<Rect> detect(Bitmap bitmap, String alg) {
        if (isInitiated()) {
            Rect[] faces = jniDetect(bitmap, alg);
            return Arrays.asList(faces);
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

        private Context mContext;

        InitThread(Context context) {
            super("InitThread");
            mContext = context;
        }

        public void run() {
            init();
        }

    }
}
