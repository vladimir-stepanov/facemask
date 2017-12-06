package com.hfs.furyclient.utils;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.widget.TextView;

import com.hfs.furyclient.opencv.HFSObjTracker;

import java.util.List;

/**
 * Created by hfs on 06.12.2017.
 */

public class HFSObjectTracker {

    private static final Paint sLandmarkPaint = new Paint();
    private static HFSObjTracker sObjTracker;
    private static long sRecognitionTime;
    private static float sRecognitionTimeSum;
    private static long sNativeRecognitionTime;
    private static float sNativeRecognitionTimeSum;
    private static long sRecognizedFrameCount;
    private static long sFrameCount;

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

    static void clearStatistics() {
        sRecognitionTime = 0;
        sRecognitionTimeSum = 0;
        sNativeRecognitionTime = 0;
        sNativeRecognitionTimeSum = 0;
        sRecognizedFrameCount = 0;
        sFrameCount = 0;
    }

    static void setLandmarksDetection() {
        clearStatistics();
        sObjTracker = HFSObjTracker.getInstance();
    }

    @SuppressWarnings("UnusedParameters")
    static void detectObject(final Activity activity, Bitmap bitmap, String alg) {
        List<Rect> objs = null;
        if (sObjTracker != null) {
            if (sObjTracker.isInitiated()) {
                final long startTime = System.currentTimeMillis();
                objs = sObjTracker.detect(bitmap, alg);
                final long endTime = System.currentTimeMillis();
                sRecognitionTime = endTime - startTime;
                sRecognitionTimeSum += sRecognitionTime;
                sNativeRecognitionTime = sObjTracker.mSpentTime;
                sNativeRecognitionTimeSum += sNativeRecognitionTime;
                sFrameCount++;
                if (objs != null && objs.size() > 0) {
                    sRecognizedFrameCount++;
                }
            }
        }

        if (objs != null) {
            for (Rect obj : objs) {
                // Draw one obj only on preview bitmap
                Canvas canvas = new Canvas(bitmap);
                float scale = (float) bitmap.getHeight() / 192;
                if (scale < 1) {
                    scale = 1.0f;
                }
                // Draw box
                sLandmarkPaint.setStrokeWidth(scale);
                canvas.drawRect(obj, sLandmarkPaint);
            }
        }
    }
}
