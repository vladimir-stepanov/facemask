package com.hfs.furyclient.utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
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
    public static List<Rect> sRectList = new ArrayList<>();
    private final Point mScreenSize = new Point();
    private Handler mInferenceHandler;
    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;
    private byte[][] mYUVBytes;
    private int[] mRGBBytes = null;
    private Bitmap mRGBFrameBitmap = null;
    public Bitmap mCroppedBitmap = null;
    private boolean mIsComputing = false;
    private boolean mOperational;
    private Activity mActivity;
    private int mCameraFacing = CameraCharacteristics.LENS_FACING_BACK;
    private HFSDrawingView mDrawingView;
    private TrackerFocusListener mListener;
    public Image image = null;
    private ArrayList<Rect> sRectResList = new ArrayList<>();;

    public void initialize(Activity activity, Handler handler, HFSDrawingView view, List<Rect> rectList) {
        mInferenceHandler = handler;
        mActivity = activity;
        mDrawingView = view;
        sRectList = rectList;
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

    public void setListener(TrackerFocusListener listener){
        mListener = listener;
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

        outBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
        return outBmp;
    }

    @Override
    public void onImageAvailable(ImageReader reader) {

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
                        if (sRectResList.isEmpty()) {
                            if(sRectList != null && !sRectList.isEmpty()) {
                                sRectResList.addAll(sRectList);
                                HFSObjectTracker.initTracker(mCroppedBitmap, sRectResList);
                            }
                        } else {
                            sRectResList.clear();
                            List<Rect> objs = HFSObjectTracker.trackObject(mCroppedBitmap);
                            sRectResList.addAll(objs);
                        }
                        mActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (sRectResList != null && sRectResList.size() >= 2) {
                                    if (mListener != null) {
                                        mListener.onObjectsLosted();
                                    }
                                }
                                mDrawingView.updateObjects(sRectResList);
                            }
                        });

                        mIsComputing = false;
                    }
                });
    }

    public interface TrackerFocusListener {
        void onObjectsLosted();
    }
}
