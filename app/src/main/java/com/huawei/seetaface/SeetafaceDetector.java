package com.huawei.seetaface;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import com.huawei.dlib.DlibDetectedFace;
import com.huawei.facemask.R;

import java.io.File;
import java.util.Arrays;
import java.util.List;

@SuppressWarnings("JniMissingFunction")
public class SeetafaceDetector {

    private static final String TAG = "SeetafaceDetector";
    private static final Object LOCK = new Object();
    private static final String CASCADE = "Facemask/seeta_fa_v1.1.bin";
    private static final String CASCADE2 = "Facemask/seeta_fd_frontal_v1.0.bin";
    @SuppressLint("StaticFieldLeak")
    private static SeetafaceDetector sSeetafaceDetector = null;

    static {
        try {
            System.loadLibrary("seetaface_jni");
            jniNativeClassInit();
            Log.d(TAG, "jniNativeClassInit success");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "library not found");
        }
    }

    private Context mContext;
    private boolean mInitiated;
    private boolean mInitialising;
    private String mCascadeClassifierPath;
    private String mCascadeClassifierPath2;
    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeFaceDetectorContext;
    public long mSpentTime;
    public long mDetectLandmarks;

    private SeetafaceDetector(Context context) {
        mInitiated = false;
        mInitialising = false;
        mContext = context;
        mCascadeClassifierPath = new File(Environment.getExternalStorageDirectory(),
                CASCADE).getAbsolutePath();
        mCascadeClassifierPath2 = new File(Environment.getExternalStorageDirectory(),
                CASCADE2).getAbsolutePath();
    }

    public static SeetafaceDetector getInstance(Context context) {
        synchronized (LOCK) {
            if (sSeetafaceDetector == null) {
                sSeetafaceDetector = new SeetafaceDetector(context);
            }
            return sSeetafaceDetector;
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

    private void init(Context context) {
        if (isInitialising()) {
            return;
        }
        setInitialising(true);
        if (!new File(mCascadeClassifierPath).exists()) {
            Toast.makeText(context, context.getString(R.string.file_not_found,
                    CASCADE), Toast.LENGTH_LONG).show();
            return;
        }
        jniInit(mCascadeClassifierPath, mCascadeClassifierPath2);
        setInitiated(true);
        setInitialising(false);
    }

    public void asyncInit() {
        new InitThread(mContext).start();
    }

    public List<DlibDetectedFace> detect(Bitmap bitmap) {
        if (isInitiated()) {
            DlibDetectedFace[] faces = jniBitmapDetect(bitmap);
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
        jniDeInit();
    }

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniInit(String path1, String path2);

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniDeInit();

    private synchronized native DlibDetectedFace[] jniBitmapDetect(Bitmap bitmap);

    private class InitThread extends Thread {

        private Context mContext;

        InitThread(Context context) {
            super("InitThread");
            mContext = context;
        }

        public void run() {
            init(mContext);
        }

    }
}
