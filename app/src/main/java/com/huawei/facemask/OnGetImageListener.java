package com.huawei.facemask;

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

/**
 * Class that takes in preview frames and converts the image to Bitmaps to process with dlib lib.
 */
class OnGetImageListener implements OnImageAvailableListener {

    private static final String TAG = "OnGetImageListener";
    private static final boolean GRAY = false;
    private static final boolean CONTRAST = true;
    static final int DLIB_FACE_RECOGNITION = 1;
    static final int GMS_FACE_RECOGNITION = 2;
    static float SCALE = 0.5f;
    private final Point mScreenSize = new Point();
    private Activity mActivity;
    //    private FaceView mFaceView;
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
    private long mLastElapsedTime = 0;
    private float mLastFrameRate = 0;
    private FloatingPreviewWindow mFloatingWindow;
    private float mBrightness;
    private float mContrast;
    private int mFaceRecognition = DLIB_FACE_RECOGNITION;
    private boolean mOperational;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;

    //    FloatingPreviewWindow initialize(Activity activity, FaceView faceView,
//                                     TextView score, TextView frameRate,
//                                     TextView mouthOpen, boolean detectLandmarks,
//                                     Handler handler) {
    void initialize(Activity activity, FloatingPreviewWindow floatingWindow,
                    TextView score, TextView frameRate,
                    TextView mouthOpen, boolean detectLandmarks,
                    Handler handler) {
        mActivity = activity;
//        mFaceView = faceView;
        mScore = score;
        mFrameRate = frameRate;
        mMouthOpen = mouthOpen;
        mInferenceHandler = handler;
        // Get Dlib face detector
        mFloatingWindow = floatingWindow;
        Display display = ((WindowManager) activity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);
        GmsVision.setLandmarksDetection(activity, detectLandmarks);
        Dlib.setLandmarksDetection(detectLandmarks);
    }

    void setOperational(boolean value) {
        mOperational = value;
    }

    private boolean isOperational() {
        return mOperational;
    }

    void setLandmarksDetection(boolean detectLandmarks) {
        GmsVision.setLandmarksDetection(mActivity, detectLandmarks);
    }

    void setBrightness(float brightness) {
        mBrightness = brightness;
    }

    void setContrast(float contrast) {
        mContrast = contrast;
    }

    void setCameraFacing(int cameraFacing) {
        mCameraFacing = cameraFacing;
    }

    void setFaceRecognition(int recognition) {
        mFaceRecognition = recognition;
    }

    private Bitmap imageSideInversion(Bitmap src) {
        Matrix sideInversion = new Matrix();
        if (mCameraFacing == CameraCharacteristics.LENS_FACING_FRONT) {
            sideInversion.setScale(SCALE, -SCALE);
            sideInversion.postRotate(-90);
        } else {
            sideInversion.setScale(SCALE, SCALE);
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
        if (!isOperational()) {
            reader.acquireLatestImage().close();
            return;
        }
        Image image = null;
        long elapsedTime = SystemClock.elapsedRealtime();

        if (mLastElapsedTime != 0) {
            float frameRate = 1000f / (elapsedTime - mLastElapsedTime);
            if (mLastFrameRate == 0) {
                mLastFrameRate = frameRate;
            } else {
                mLastFrameRate = ((mLastFrameRate * 29) + frameRate) / 30;
            }
            mActivity.runOnUiThread(
                    new Runnable() {
                        @Override
                        public void run() {
                            mFrameRate.setText(mActivity.getResources().getString(
                                    R.string.camera_preview_frame_rate, mLastFrameRate));
                        }
                    });
        }
        mLastElapsedTime = elapsedTime;

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
                                Dlib.detectFace(mActivity, mScore, mMouthOpen, mCroppedBitmap);
                                break;
                            case GMS_FACE_RECOGNITION:
                                GmsVision.detectFace(mActivity, mScore, mCroppedBitmap);
                                break;
                        }
                        mFloatingWindow.setRGBBitmap(mCroppedBitmap);
                        mIsComputing = false;
                    }
                });
    }

}
