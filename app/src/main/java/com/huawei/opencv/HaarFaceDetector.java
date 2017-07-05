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

@SuppressWarnings("JniMissingFunction")
public class HaarFaceDetector {

    private static final String TAG = "HaarFaceDetector";
    private static final Object LOCK = new Object();
    private static final String CASCADE = "Facemask/haarcascade_frontalface_alt.xml";
    @SuppressLint("StaticFieldLeak")
    private static HaarFaceDetector sHaarFaceDetector = null;

    static {
        try {
            System.loadLibrary("haar_jni");
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
    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeFaceDetectorContext;

    private HaarFaceDetector(Context context) {
        mInitiated = false;
        mInitialising = false;
        mContext = context;
        mCascadeClassifierPath = new File(Environment.getExternalStorageDirectory(),
                CASCADE).getAbsolutePath();
    }

    public static HaarFaceDetector getInstance(Context context) {
        synchronized (LOCK) {
            if (sHaarFaceDetector == null) {
                sHaarFaceDetector = new HaarFaceDetector(context);
            }
            return sHaarFaceDetector;
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
        jniInit(mCascadeClassifierPath);
        setInitiated(true);
        setInitialising(false);
    }

    public void asyncInit() {
        new InitThread(mContext).start();
    }

    public List<Rect> detect(Bitmap bitmap) {
        if (isInitiated()) {
            Rect[] faces = jniBitmapDetect(bitmap);
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
    private synchronized native int jniInit(String cascadeClassifierPath);

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniDeInit();

    private synchronized native Rect[] jniBitmapDetect(Bitmap bitmap);

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
