package com.huawei.dlib;

import android.graphics.Point;
import android.graphics.Rect;

import java.util.ArrayList;

/**
 * A DlibDetectedFace contains all the information identifying the location
 * and confidence value of the detected object in a bitmap.
 */
@SuppressWarnings("CanBeFinal")
public final class DlibDetectedFace {

    public final float[] mAngles = new float[3];
    public final float[] mModelView = new float[16];
    public final float[] mFrustumScale = new float[3];
    public final float[] mLandmarks = new float[2 * 68];
    private final ArrayList<Point> mLandmarkPoints = new ArrayList<>();
    private int mLeft = 0;
    private int mTop = 0;
    private int mRight = 0;
    private int mBottom = 0;
    @SuppressWarnings("FieldCanBeLocal")
    private int mNoseEndX = 0;
    @SuppressWarnings("FieldCanBeLocal")
    private int mNoseEndY = 0;
    @SuppressWarnings("FieldCanBeLocal")
    private float mMouthOpen = 0;

    @SuppressWarnings("unused")
        // Used by jni
    DlibDetectedFace() {
    }

    public Rect getBounds() {
        return new Rect(mLeft, mTop, mRight, mBottom);
    }

    public Point getNoseStart() {
        return mLandmarkPoints.get(30);
    }

    public Point getNoseEnd() {
        return new Point(mNoseEndX, mNoseEndY);
    }

    public float getMouthOpen() {
        return mMouthOpen;
    }

    /**
     * Add landmark to the list. Usually, call by jni
     *
     * @param x Point x
     * @param y Point y
     * @return true if adding landmark successfully
     */
    @SuppressWarnings("unused") // It is used from native
    public boolean addLandmark(int x, int y) {
        return mLandmarkPoints.add(new Point(x, y));
    }

    /**
     * Return the list of landmark points
     *
     * @return ArrayList of android.graphics.Point
     */
    public ArrayList<Point> getFaceLandmarks() {
        return mLandmarkPoints;
    }

    @Override
    public String toString() {
        return "Left:" + mLeft + ", Top:" + mTop + ", Right:" + mRight + ", Bottom:" + mBottom;
    }
}
