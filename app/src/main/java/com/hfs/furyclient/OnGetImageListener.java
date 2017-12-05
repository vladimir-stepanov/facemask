package com.hfs.furyclient;

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

import com.hfs.opencv.ImageUtils;
import com.hfs.opencv.ObjTracker;

/**
 * Class that takes in preview frames and converts the image to Bitmaps to process with dlib lib.
 */
@SuppressWarnings("FieldCanBeLocal")
class OnGetImageListener implements OnImageAvailableListener {

    static final int MIL_TRACKER = 6;
    static final int KCF_TRACKER = 7;
    static final int BOOSTING_TRACKER = 8;
    static final int MEDIANFLOW_TRACKER = 9;
    static final int TLD_TRACKER = 10;
    private static final String TAG = "OnGetImageListener";
    private static final boolean CONTRAST = true;
    static float scale = 0.5f;
    private final Point mScreenSize = new Point();
    private Activity mActivity;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    private Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private Handler mInferenceHandler;
    private TextView mScore;
    private TextView mFrameRate;
    private static long sLastElapsedTime = 0;
    private static float sLastFrameRate = 0;
    private FloatingPreviewWindow mFloatingWindow;
    private final float mBrightness = 0f;
    private final float mContrast = 1.0f;
    private int mObjRecognition = MEDIANFLOW_TRACKER;
    private boolean mOperational;
    private static long sFrameCount;
    private static float sFrameSum;
    private static float sAverageFrameRate;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;

    void initialize(Activity activity, FloatingPreviewWindow floatingWindow,
                    TextView score, TextView frameRate,
                    Handler handler) {
        mActivity = activity;
        mScore = score;
        mFrameRate = frameRate;
        mInferenceHandler = handler;
        mFloatingWindow = floatingWindow;
        WindowManager wm = (WindowManager) activity.getSystemService(Context.WINDOW_SERVICE);
        if (wm == null) {
            mScreenSize.set(1080, 1920);
        } else {
            Display display = wm.getDefaultDisplay();
            display.getRealSize(mScreenSize);
        }
        ObjectTracker.setLandmarksDetection();
    }

    private boolean isOperational() {
        return mOperational;
    }

    static void clearStatistics() {
        sLastElapsedTime = 0;
        sLastFrameRate = 0;
        sFrameCount = 0;
        sFrameSum = 0;
        ObjectTracker.clearStatistics();
    }

    void setOperational(boolean value) {
        clearStatistics();
        mOperational = value;
    }

    void setCameraFacing(int cameraFacing) {
        clearStatistics();
        mCameraFacing = cameraFacing;
    }

    void setObjRecognition(int recognition) {
        clearStatistics();
        mObjRecognition = recognition;
    }

    private Bitmap imageSideInversion(Bitmap src) {
        Matrix sideInversion = new Matrix();
        if (mCameraFacing == CameraCharacteristics.LENS_FACING_FRONT) {
            sideInversion.setScale(scale, -scale);
            sideInversion.postRotate(-90);
        } else {
            sideInversion.setScale(scale, scale);
            sideInversion.postRotate(-90);
        }
        Bitmap outBmp;
        if (CONTRAST) {
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

        if (sLastElapsedTime != 0 && (elapsedTime > sLastElapsedTime)) {
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
                    uvPixelStride);

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
                        switch (mObjRecognition) {
                            case MIL_TRACKER:
                                ObjectTracker.detectObject(mActivity, mScore, mCroppedBitmap, ObjTracker.MIL_TRACKER_ALGORITHM);
                                break;
                            case BOOSTING_TRACKER:
                                ObjectTracker.detectObject(mActivity, mScore, mCroppedBitmap, ObjTracker.BOOSTING_TRACKER_ALGORITHM);
                                break;
                            case TLD_TRACKER:
                                ObjectTracker.detectObject(mActivity, mScore, mCroppedBitmap, ObjTracker.TLD_TRACKER_ALGORITHM);
                                break;
                            case MEDIANFLOW_TRACKER:
                                ObjectTracker.detectObject(mActivity, mScore, mCroppedBitmap, ObjTracker.MEDIANFLOW_TRACKER_ALGORITHM);
                                break;
                            case KCF_TRACKER:
                                ObjectTracker.detectObject(mActivity, mScore, mCroppedBitmap, ObjTracker.KCF_TRACKER_ALGORITHM);
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
