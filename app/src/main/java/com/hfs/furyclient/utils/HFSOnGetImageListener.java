package com.hfs.furyclient.utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Point;
import android.hardware.camera2.CameraCharacteristics;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.view.Display;
import android.view.WindowManager;

import com.hfs.furyclient.opencv.HFSImageUtils;
import com.hfs.furyclient.opencv.HFSObjTracker;

/**
 * Created by hfs on 06.12.2017.
 */

public class HFSOnGetImageListener implements ImageReader.OnImageAvailableListener {
    public static final int MEDIANFLOW_TRACKER = 9;
    public static float scale = 0.5f;
    private static long sFrameCount;
    private static float sFrameSum;
    private static float sAverageFrameRate;
    private final Point mScreenSize = new Point();
    private final float mBrightness = 0f;
    private final float mContrast = 1.0f;
    private Activity mActivity;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    private Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private Handler mInferenceHandler;
    private int mObjRecognition = MEDIANFLOW_TRACKER;
    private boolean mOperational;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_BACK;

    public static void clearStatistics() {
        sFrameCount = 0;
        sFrameSum = 0;
        HFSObjectTracker.clearStatistics();
    }

    public void initialize(Activity activity, Handler handler) {
        mActivity = activity;
        mInferenceHandler = handler;
        WindowManager wm = (WindowManager) activity.getSystemService(Context.WINDOW_SERVICE);
        if (wm == null) {
            mScreenSize.set(1080, 1920);
        } else {
            Display display = wm.getDefaultDisplay();
            display.getRealSize(mScreenSize);
        }
        HFSObjectTracker.setLandmarksDetection();
    }

    private boolean isOperational() {
        return mOperational;
    }

    public void setOperational(boolean value) {
        clearStatistics();
        mOperational = value;
    }

    public void setCameraFacing(int cameraFacing) {
        clearStatistics();
        mCameraFacing = cameraFacing;
    }

    public void setObjRecognition(int recognition) {
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

        outBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
//        if (CONTRAST) {
//            // contrast = 0..10 1 is default
//            // brightness = -255..255 0 is default
//            Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
//            outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
//            Canvas canvas = new Canvas(outBmp);
//            ColorMatrix cm = new ColorMatrix(new float[]{
//                    mContrast, 0, 0, 0, mBrightness,
//                    0, mContrast, 0, 0, mBrightness,
//                    0, 0, mContrast, 0, mBrightness,
//                    0, 0, 0, 1, 0
//            });
//            Paint paint = new Paint();
//            paint.setColorFilter(new ColorMatrixColorFilter(cm));
//            canvas.drawBitmap(inBmp, 0, 0, paint);
//        } else {
//        }
        return outBmp;
    }

    @Override
    public void onImageAvailable(ImageReader reader) {
        Image image = null;
        if (!isOperational()) {
            image = reader.acquireLatestImage();
            if (image != null) {
                image.close();
            }
            return;
        }

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

            final Image.Plane[] planes = image.getPlanes();

            // Initialize the storage bitmaps once when the resolution is known.
            if (mPreviewWidth != image.getWidth() || mPreviewHeight != image.getHeight()) {
                mPreviewWidth = image.getWidth();
                mPreviewHeight = image.getHeight();

                mRGBBytes = new int[mPreviewWidth * mPreviewHeight];
                mRGBFrameBitmap = Bitmap.createBitmap(mPreviewWidth, mPreviewHeight, Bitmap.Config.ARGB_8888);

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
            HFSImageUtils.convertYUV420ToARGB8888(
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

        mInferenceHandler.post(new Runnable() {
            @Override
            public void run() {
                HFSObjectTracker.detectObject(mActivity, mCroppedBitmap, HFSObjTracker.MEDIANFLOW_TRACKER_ALGORITHM);
                mIsComputing = false;
            }
        });
    }
}
