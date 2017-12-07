package com.hfs.furyclient.utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.hardware.camera2.CameraCharacteristics;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.view.Display;
import android.view.WindowManager;

import com.hfs.furyclient.opencv.HFSObjTracker;
import com.hfs.furyclient.views.HFSDrawingView;

import java.util.ArrayList;
import java.util.List;

public class HFSOnGetImageListener implements ImageReader.OnImageAvailableListener {
    public static float scale = 0.5f;
    private static List<Rect> sRectList = new ArrayList<>();
    private final Point mScreenSize = new Point();
    private Handler mInferenceHandler;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    private Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private boolean mOperational;
    private Activity mActivity;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_BACK;
    private HFSDrawingView mDrawingView;

    public void initialize(Activity activity, Handler handler, HFSDrawingView view) {
        mInferenceHandler = handler;
        mActivity = activity;
        mDrawingView = view;
        WindowManager wm = (WindowManager) activity.getSystemService(Context.WINDOW_SERVICE);
        if (wm == null) {
            mScreenSize.set(1080, 1920);
        } else {
            Display display = wm.getDefaultDisplay();
            display.getRealSize(mScreenSize);
        }
    }

    private boolean isOperational() {
        return mOperational;
    }

    public void setOperational(boolean value) {
        mOperational = value;
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
            HFSObjTracker.convertYUV420ToARGB8888(
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
                        if (sRectList.isEmpty()) {
                            Rect rect = new Rect(40, 60, 60, 90);
                            sRectList.add(rect);
                            rect = new Rect(100, 140, 130, 160);
                            sRectList.add(rect);
                            HFSObjectTracker.initTracker(mCroppedBitmap, sRectList);
                        } else {
                            sRectList.clear();
                            List<Rect> objs = HFSObjectTracker.trackObject(mCroppedBitmap);
                            sRectList.addAll(objs);
                        }
                        mActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mDrawingView.updateObjects(sRectList);
                            }
                        });

                        mIsComputing = false;
                    }
                });
    }
}
