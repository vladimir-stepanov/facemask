package com.huawei.dlib;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import com.huawei.facemask.R;

import java.io.File;
import java.util.Arrays;
import java.util.List;

@SuppressWarnings("JniMissingFunction")
public class DlibModFaceDetector {

    private static final String TAG = "DlibModFaceDetector";
    private static final Object LOCK = new Object();
    private static final String PREDICTOR = "Facemask/shape_predictor_68_face_landmarks.dat";
    private static final String CASCADE = "Facemask/lbpcascade_frontalface.xml";
    @SuppressLint("StaticFieldLeak")
    private static DlibModFaceDetector sDlibFaceDetector = null;

    static {
        try {
            System.loadLibrary("dlib_mod_jni");
            jniNativeClassInit();
            Log.d(TAG, "jniNativeClassInit success");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "library not found");
        }
    }

    private Context mContext;
    private boolean mInitiated;
    private boolean mInitialising;
    private String mFaceShapeModelPath;
    // accessed by native methods
    @SuppressWarnings("unused")
    private long mNativeFaceDetectorContext;
    private long mNativeFaceDetectorContext2;
    public long mSpentTime;
    public long mDetectLandmarks;

    private DlibModFaceDetector(Context context) {
        mInitiated = false;
        mInitialising = false;
        mContext = context;
        mFaceShapeModelPath = new File(Environment.getExternalStorageDirectory(),
                PREDICTOR).getAbsolutePath();
    }

    public static DlibModFaceDetector getInstance(Context context) {
        synchronized (LOCK) {
            if (sDlibFaceDetector == null) {
                sDlibFaceDetector = new DlibModFaceDetector(context);
            }
            return sDlibFaceDetector;
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
        if (!new File(mFaceShapeModelPath).exists()) {
            Toast.makeText(context, context.getString(R.string.file_not_found,
                    PREDICTOR), Toast.LENGTH_LONG).show();
            return;
        }
        jniInit(mFaceShapeModelPath,
                new File(Environment.getExternalStorageDirectory(), CASCADE).getAbsolutePath());
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
    private synchronized native int jniInit(String landmarkModelPath, String cascadePath);

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniDeInit();

    private synchronized native DlibDetectedFace[] jniBitmapDetect(Bitmap bitmap);

    private class InitThread extends Thread {
        Context mContext;

        InitThread(Context context) {
            super("InitThread");
            mContext = context;
        }

        public void run() {
            init(mContext);
        }

    }
}
