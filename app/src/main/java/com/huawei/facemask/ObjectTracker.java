package com.huawei.facemask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.widget.TextView;

import com.huawei.opencv.ObjTracker;

import java.util.List;

class ObjectTracker {

    private static final Paint sLandmarkPaint = new Paint();
    @SuppressLint("StaticFieldLeak")
    private static ObjTracker sObjTracker;
    @SuppressWarnings({"FieldCanBeLocal", "unused"})
    private static boolean sDetectLandmarks;
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

    static void release() {
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

    static void setLandmarksDetection(Context context) {
        clearStatistics();
        sObjTracker = ObjTracker.getInstance(context);
    }

    @SuppressWarnings("UnusedParameters")
    static void detectObject(final Activity activity, final TextView score,
                             final TextView mouth, Bitmap bitmap, String alg) {
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

                activity.runOnUiThread(
                        new Runnable() {
                            @SuppressLint("StringFormatMatches")
                            @Override
                            public void run() {
                                float percent = 0;
                                float average = 0;
                                float nativeAverage = 0;
                                if (sFrameCount != 0) {
                                    percent = (float) sRecognizedFrameCount * 100 / sFrameCount;
                                    average = sRecognitionTimeSum / sFrameCount;
                                    nativeAverage = sNativeRecognitionTimeSum / sFrameCount;
                                }
                                score.setText(activity.getResources().getString(
                                        R.string.object_recognition_time,
                                        sRecognitionTime, average, nativeAverage, percent));
                            }
                        });
            } else {
                activity.runOnUiThread(
                        new Runnable() {
                            @Override
                            public void run() {
                                score.setText(activity.getResources().getString(R.string.initializing_engine));
                            }
                        });
            }
        }

        if (objs != null && objs.size() > 0) {
            // Draw one obj only on preview bitmap
            Rect obj = objs.get(0);
            Canvas canvas = new Canvas(bitmap);
            float scale = (float) bitmap.getHeight() / 192;
            if (scale < 1) {
                scale = 1.0f;
            }
            // Draw box
            sLandmarkPaint.setColor(Color.RED);
            sLandmarkPaint.setStrokeWidth(scale);
            canvas.drawRect(obj, sLandmarkPaint);
        }
    }
}
