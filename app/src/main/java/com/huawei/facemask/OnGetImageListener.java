package com.huawei.facemask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
import android.hardware.camera2.CameraCharacteristics;
import android.media.Image;
import android.media.Image.Plane;
import android.media.ImageReader;
import android.media.ImageReader.OnImageAvailableListener;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;
import android.widget.TextView;

import com.huawei.dlib.ImageUtils;

import grapevine.face1.FaceView;

/**
 * Class that takes in preview frames and converts the image to Bitmaps to process with dlib lib.
 */
class OnGetImageListener implements OnImageAvailableListener {
    static final int DLIB_MOD_FACE_RECOGNITION = 0;
    static final int DLIB_FACE_RECOGNITION = 1;
    static final int GMS_FACE_RECOGNITION = 2;
    static final int HAAR_FACE_RECOGNITION = 3;
    static final int LBP_FACE_RECOGNITION = 4;
    static final int SEETAFACE_RECOGNITION = 5;
    private static final String TAG = "OnGetImageListener";
    private static final boolean GRAY = false;
    private static final boolean CONTRAST = true;
    static float scale = 0.5f;
    private final Point mScreenSize = new Point();
    private Activity mActivity;
    private FaceView mFaceView;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    private Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private Handler mInferenceHandler;
    // Dlib face detector
    private TextView mScore;
    private TextView mFrameRate;
    private TextView mMouthOpen;
    private static long sLastElapsedTime = 0;
    private static float sLastFrameRate = 0;
    private FloatingPreviewWindow mFloatingWindow;
    private float mBrightness;
    private float mContrast;
    private int mFaceRecognition = DLIB_FACE_RECOGNITION;
    private boolean mOperational;
    private static long sFrameCount;
    private static float sFrameSum;
    private static float sAverageFrameRate;
    private boolean mShowMask;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;

    void initialize(Activity activity, FaceView faceView, FloatingPreviewWindow floatingWindow,
                    TextView score, TextView frameRate,
                    TextView mouthOpen, boolean detectLandmarks,
                    Handler handler) {
        mActivity = activity;
        mFaceView = faceView;
        mScore = score;
        mFrameRate = frameRate;
        mMouthOpen = mouthOpen;
        mInferenceHandler = handler;
        // Get Dlib face detector
        mFloatingWindow = floatingWindow;
        Display display = ((WindowManager) activity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);
        Dlib.setLandmarksDetection(activity, detectLandmarks);
        DlibMod.setLandmarksDetection(activity, detectLandmarks);
        Seetaface.setLandmarksDetection(activity, detectLandmarks);
        GmsVision.setLandmarksDetection(activity, detectLandmarks);
        HaarCascade.setLandmarksDetection(activity, detectLandmarks);
        LbpCascade.setLandmarksDetection(activity, detectLandmarks);
    }

    private boolean isOperational() {
        return mOperational;
    }

    static void clearStatistics() {
        sLastElapsedTime = 0;
        sLastFrameRate = 0;
        sFrameCount = 0;
        sFrameSum = 0;
        Dlib.clearStatistics();
        DlibMod.clearStatistics();
        GmsVision.clearStatistics();
    }

    void setOperational(boolean value) {
        clearStatistics();
        mOperational = value;
    }

    void setShowMask(boolean showMask) {
        mShowMask = showMask;
        if (!mShowMask) {
            if (mFaceView != null) {
                mFaceView.setPoseAnglesAndModelView(false, null, null, null, null);
            }
        }
    }

    void setBrightness(float brightness) {
        clearStatistics();
        mBrightness = brightness;
    }

    void setContrast(float contrast) {
        clearStatistics();
        mContrast = contrast;
    }

    void setCameraFacing(int cameraFacing) {
        clearStatistics();
        mCameraFacing = cameraFacing;
    }

    void setFaceRecognition(int recognition) {
        clearStatistics();
        mFaceRecognition = recognition;
    }

    private Bitmap imageSideInversion(Bitmap src) {
        Matrix sideInversion = new Matrix();
        if (mCameraFacing == CameraCharacteristics.LENS_FACING_FRONT) {
            sideInversion.setScale(scale, -scale);
            sideInversion.postRotate(-90);
        } else {
            sideInversion.setScale(scale, scale);
            sideInversion.postRotate(90);
        }
        Bitmap outBmp;
        if (GRAY) {
            Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
            outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
            Canvas c = new Canvas(outBmp);
            Paint paint = new Paint();
            ColorMatrix cm = new ColorMatrix();
            cm.setSaturation(0);
            ColorMatrixColorFilter f = new ColorMatrixColorFilter(cm);
            paint.setColorFilter(f);
            c.drawBitmap(inBmp, 0, 0, paint);
        } else if (CONTRAST) {
            // contrast = 0..10 1 is default
            // brightness = -255..255 0 is default
            Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
            outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(outBmp);
            ColorMatrix cm = new ColorMatrix(new float[]{
                    mContrast, 0, 0, 0, mBrightness,
                    0, mContrast, 0, 0, mBrightness,
                    0, 0, mContrast, 0, mBrightness,
                    0, 0, 0, 1, 0
            });
            Paint paint = new Paint();
            paint.setColorFilter(new ColorMatrixColorFilter(cm));
            canvas.drawBitmap(inBmp, 0, 0, paint);
        } else {
            outBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
        }
        return outBmp;
    }

    @Override
    public void onImageAvailable(final ImageReader reader) {
        Image image = null;
        if (!isOperational()) {
            image = reader.acquireLatestImage();
            if (image != null) {
                image.close();
            }
            return;
        }

        long elapsedTime = SystemClock.elapsedRealtime();

        if (sLastElapsedTime != 0) {
            float frameRate = 1000f / (elapsedTime - sLastElapsedTime);
            if (sLastFrameRate == 0) {
                sLastFrameRate = frameRate;
            } else {
                sFrameSum += frameRate;
                sFrameCount++;
                sAverageFrameRate = sFrameSum / sFrameCount;
                sLastFrameRate = (sLastFrameRate + frameRate) / 2;
            }
            mActivity.runOnUiThread(
                    new Runnable() {
                        @SuppressLint("StringFormatMatches")
                        @Override
                        public void run() {
                            mFrameRate.setText(mActivity.getResources().getString(
                                    R.string.camera_preview_frame_rate, sLastFrameRate, sAverageFrameRate));
                        }
                    });
        }
        sLastElapsedTime = elapsedTime;

        try {
            image = reader.acquireLatestImage();

            if (image == null) {
                return;
            }

            if (mIsComputing) {
                image.close();
                return;
            }
            mIsComputing = true;

            final Plane[] planes = image.getPlanes();

            // Initialize the storage bitmaps once when the resolution is known.
            if (mPreviewWidth != image.getWidth() || mPreviewHeight != image.getHeight()) {
                mPreviewWidth = image.getWidth();
                mPreviewHeight = image.getHeight();

                Log.d(TAG, String.format("Initializing at size %dx%d", mPreviewWidth, mPreviewHeight));
                mRGBBytes = new int[mPreviewWidth * mPreviewHeight];
                mRGBFrameBitmap = Bitmap.createBitmap(mPreviewWidth, mPreviewHeight, Config.ARGB_8888);

                mYUVBytes = new byte[planes.length][];
                for (int i = 0; i < planes.length; ++i) {
                    mYUVBytes[i] = new byte[planes[i].getBuffer().capacity()];
                }
            }

            for (int i = 0; i < planes.length; ++i) {
                planes[i].getBuffer().get(mYUVBytes[i]);
            }

            final int yRowStride = planes[0].getRowStride();
            final int uvRowStride = planes[1].getRowStride();
            final int uvPixelStride = planes[1].getPixelStride();
            ImageUtils.convertYUV420ToARGB8888(
                    mYUVBytes[0],
                    mYUVBytes[1],
                    mYUVBytes[2],
                    mRGBBytes,
                    mPreviewWidth,
                    mPreviewHeight,
                    yRowStride,
                    uvRowStride,
                    uvPixelStride,
                    false);

            image.close();
        } catch (final Exception e) {
            if (image != null) {
                image.close();
            }
            return;
        }

        mRGBFrameBitmap.setPixels(mRGBBytes, 0, mPreviewWidth, 0, 0, mPreviewWidth, mPreviewHeight);
        mCroppedBitmap = imageSideInversion(mRGBFrameBitmap);

        mInferenceHandler.post(
                new Runnable() {
                    @Override
                    public void run() {
                        switch (mFaceRecognition) {
                            case DLIB_FACE_RECOGNITION:
                                Dlib.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                            case DLIB_MOD_FACE_RECOGNITION:
                                DlibMod.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                            case GMS_FACE_RECOGNITION:
                                GmsVision.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                            case SEETAFACE_RECOGNITION:
                                Seetaface.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                            case HAAR_FACE_RECOGNITION:
                                HaarCascade.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                            case LBP_FACE_RECOGNITION:
                                LbpCascade.detectFace(mActivity, mFaceView, mScore, mMouthOpen, mCroppedBitmap, mShowMask);
                                break;
                        }
                        if (isOperational()) {
                            mFloatingWindow.setRGBBitmap(mCroppedBitmap);
                        }
                        mIsComputing = false;
                    }
                });
    }

}
