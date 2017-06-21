package com.huawei.dlib;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.List;

@SuppressWarnings("JniMissingFunction")
public class DlibFaceDetector {

    private static final String TAG = "DlibFaceDetector";
    private static final Object LOCK = new Object();
    @SuppressLint("StaticFieldLeak")
    private static DlibFaceDetector sDlibFaceDetector = null;

    static {
        try {
            System.loadLibrary("dlib_jni");
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

    private DlibFaceDetector(Context context) {
        mInitiated = false;
        mInitialising = false;
        mContext = context;
        mFaceShapeModelPath = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "shape_predictor_68_face_landmarks.dat";
    }

    public static DlibFaceDetector getInstance() {
        synchronized (LOCK) {
            return sDlibFaceDetector;
        }
    }

    public static DlibFaceDetector getInstance(Context context) {
        synchronized (LOCK) {
            if (sDlibFaceDetector == null) {
                sDlibFaceDetector = new DlibFaceDetector(context);
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

    private void copyFileFromRaw(int id, String targetPath) {
        InputStream in = mContext.getResources().openRawResource(id);
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(targetPath);
            byte[] buff = new byte[1024];
            int read;
            while ((read = in.read(buff)) > 0) {
                out.write(buff, 0, read);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (in != null) {
                    in.close();
                }
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void init() {
        if (isInitialising()) {
            return;
        }
        setInitialising(true);
        if (!new File(mFaceShapeModelPath).exists()) {
            //copyFileFromRaw(R.raw.shape_predictor_68_face_landmarks, mFaceShapeModelPath);
        }
        jniInit(mFaceShapeModelPath);
        setInitiated(true);
        setInitialising(false);
    }

    public void asyncInit() {
        new InitThread().start();
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
    private synchronized native int jniInit(String landmarkModelPath);

    @SuppressWarnings("UnusedReturnValue")
    private synchronized native int jniDeInit();

    private synchronized native DlibDetectedFace[] jniBitmapDetect(Bitmap bitmap);

    private class InitThread extends Thread {

        InitThread() {
            super("InitThread");
        }

        public void run() {
            init();
        }

    }
}
