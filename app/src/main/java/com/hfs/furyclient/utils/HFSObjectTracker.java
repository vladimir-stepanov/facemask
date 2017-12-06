package com.hfs.furyclient.utils;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

import com.hfs.furyclient.opencv.HFSObjTracker;

import java.util.List;

public class HFSObjectTracker {

    private static final Paint sLandmarkPaint = new Paint();
    private static HFSObjTracker sObjTracker;

    static {
        sLandmarkPaint.setColor(Color.WHITE);
        sLandmarkPaint.setStyle(Paint.Style.STROKE);
    }

    public static void release() {
        if (sObjTracker != null) {
            sObjTracker.release();
            sObjTracker = null;
        }
    }

    static void initTracker(Bitmap bitmap, List<Rect> rectList) {
        sObjTracker = HFSObjTracker.getInstance();
        sObjTracker.init(bitmap, rectList);
    }

    static List<Rect> trackObject(Bitmap bitmap) {
        List<Rect> objs = null;
        if (sObjTracker != null) {
                objs = sObjTracker.detect(bitmap);
        }
        return objs;
    }
}
