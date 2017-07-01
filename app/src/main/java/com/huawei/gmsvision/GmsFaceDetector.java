package com.huawei.gmsvision;

import android.util.SparseArray;

import com.google.android.gms.vision.Detector;
import com.google.android.gms.vision.Frame;
import com.google.android.gms.vision.face.Face;

public class GmsFaceDetector extends Detector<Face> {
    private final Detector<Face> mDelegate;

    public GmsFaceDetector(Detector<Face> delegate) {
        mDelegate = delegate;
    }

    @Override
    public void release() {
        mDelegate.release();
    }

    @Override
    public SparseArray<Face> detect(Frame frame) {
        SparseArray<Face> faces = null;
        try {
            faces = mDelegate.detect(frame);
        } catch (RuntimeException ignore) {
        }
        return faces;
    }

    @Override
    public boolean isOperational() {
        return mDelegate.isOperational();
    }

    @Override
    public boolean setFocus(int id) {
        return mDelegate.setFocus(id);
    }

}
