package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
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

import com.huawei.dlib.DetectedFace;
import com.huawei.dlib.FaceDetector;
import com.huawei.dlib.ImageUtils;

import java.util.ArrayList;
import java.util.List;

import grapevine.face1.FaceView;

/**
 * Class that takes in preview frames and converts the image to Bitmaps to process with dlib lib.
 */
class OnGetImageListener implements OnImageAvailableListener {

    private static final String TAG = "OnGetImageListener";
    private static final boolean GRAY = false;
    private static final boolean CONTRAST = false;
    private static final float SCALE = 0.4f;
    private final Point mScreenSize = new Point();
    private Activity mActivity;
    private FaceView mFaceView;
    private List<DetectedFace> mDetectedFaces;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    private Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private Handler mInferenceHandler;
    private FaceDetector mFaceDetector;
    private TextView mScore;
    private TextView mFrameRate;
    private TextView mMouthOpen;
    private long mLastElapsedTime = 0;
    private float mLastFrameRate = 0;
    private FloatingPreviewWindow mWindow;
    private Paint mFaceLandmarkPaint;

    FloatingPreviewWindow initialize(Activity activity, FaceView faceView,
                                     TextView score, TextView frameRate,
                                     TextView mouthOpen,
                                     Handler handler) {
        mActivity = activity;
        mFaceView = faceView;
        mScore = score;
        mFrameRate = frameRate;
        mMouthOpen = mouthOpen;
        mInferenceHandler = handler;
        mFaceDetector = FaceDetector.getInstance();
        mWindow = new FloatingPreviewWindow(activity);
        Display display = ((WindowManager) activity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);

        mFaceLandmarkPaint = new Paint();
        mFaceLandmarkPaint.setColor(Color.WHITE);
        mFaceLandmarkPaint.setStrokeWidth(0);
        mFaceLandmarkPaint.setStyle(Paint.Style.STROKE);
        return mWindow;
    }

    void deInitialize() {
        synchronized (OnGetImageListener.this) {
            if (mFaceDetector != null) {
                mFaceDetector.release();
            }

            if (mWindow != null) {
                mWindow.release();
            }
        }
    }

    private Bitmap imageSideInversion(Bitmap src) {
        Matrix sideInversion = new Matrix();
        sideInversion.setScale(SCALE, -SCALE);
        sideInversion.postRotate(-90);
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
            float contrast = 1;
            float brightness = 0;
            Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
            outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(outBmp);
            ColorMatrix cm = new ColorMatrix(new float[]{
                    contrast, 0, 0, 0, brightness,
                    0, contrast, 0, 0, brightness,
                    0, 0, contrast, 0, brightness,
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

                        if (mFaceDetector.isInitiated()) {
                            final long startTime = System.currentTimeMillis();
                            synchronized (OnGetImageListener.this) {
                                if (mFaceDetector != null) {
                                    mDetectedFaces = mFaceDetector.detect(mCroppedBitmap);
                                } else {
                                    mDetectedFaces = null;
                                }
                            }
                            final long endTime = System.currentTimeMillis();
                            mActivity.runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            mScore.setText(mActivity.getResources().getString(
                                                    R.string.face_recognition_time, endTime - startTime));
                                        }
                                    });
                        }

                        if (mDetectedFaces != null && mDetectedFaces.size() > 0) {
                            // Draw one face only on preview bitmap
                            DetectedFace face = mDetectedFaces.get(0);
                            Canvas canvas = new Canvas(mCroppedBitmap);
                            mFaceLandmarkPaint.setColor(Color.DKGRAY);
                            canvas.drawRect(face.getBounds(), mFaceLandmarkPaint);

                            // Draw landmark
                            mFaceLandmarkPaint.setColor(Color.WHITE);
                            ArrayList<Point> landmarks = face.getFaceLandmarks();
                            for (Point point : landmarks) {
                                canvas.drawCircle(point.x, point.y, 1.0f, mFaceLandmarkPaint);
                            }

                            mFaceLandmarkPaint.setColor(Color.BLUE);
                            Point start = face.getNoseStart();
                            Point stop = face.getNoseEnd();
                            canvas.drawLine(start.x, start.y, stop.x, stop.y, mFaceLandmarkPaint);

                            final float mouthOpen = face.getMouthOpen();
                            mActivity.runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            mMouthOpen.setText(mActivity.getResources().getString(
                                                    R.string.mouth_open, mouthOpen));
                                        }
                                    });

                            // Send face to grapevine
                            mFaceView.setPoseAnglesAndModelView(true,
                                    face.mAngles,
                                    face.mModelView,
                                    face.mFrustumScale,
                                    face.mLandmarks);
                        } else {
                            mFaceView.setPoseAnglesAndModelView(false, null, null, null, null);
                            mActivity.runOnUiThread(
                                    new Runnable() {
                                        @Override
                                        public void run() {
                                            mMouthOpen.setText(mActivity.getResources().getString(
                                                    R.string.mouth_open, 0f));
                                        }
                                    });
                        }
                        mWindow.setRGBBitmap(mCroppedBitmap);
                        mIsComputing = false;
                    }
                });

    }
}
