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

import com.huawei.opencv.LbpFaceDetector;

import java.util.List;

import grapevine.face1.FaceView;

class LbpCascade {

    private static final Paint sFaceLandmarkPaint = new Paint();
    @SuppressLint("StaticFieldLeak")
    private static LbpFaceDetector sFaceDetector;
    @SuppressWarnings({"FieldCanBeLocal", "unused"})
    private static boolean sDetectLandmarks;
    private static long sRecognitionTime;
    private static long sRecognitionTimeSum;
    private static long sRecognizedFrameCount;
    private static long sFrameCount;

    static {
        sFaceLandmarkPaint.setColor(Color.WHITE);
        sFaceLandmarkPaint.setStyle(Paint.Style.STROKE);
    }

    static void release() {
        if (sFaceDetector != null) {
            sFaceDetector.release();
        }
    }

    static void clearStatistics() {
        sRecognitionTime = 0;
        sRecognitionTimeSum = 0;
        sRecognizedFrameCount = 0;
        sFrameCount = 0;
    }

    static void setLandmarksDetection(Context context, boolean detectLandmarks) {
        clearStatistics();
        sFaceDetector = LbpFaceDetector.getInstance(context);
        sDetectLandmarks = detectLandmarks;
        // Do nothing LBP Cascade does not support landmark detection
    }

    @SuppressWarnings("UnusedParameters")
    static void detectFace(final Activity activity, FaceView faceView, final TextView score,
                           final TextView mouth, Bitmap bitmap, boolean showMask) {
        List<Rect> faces = null;
        if (sFaceDetector != null) {
            if (sFaceDetector.isInitiated()) {
                final long startTime = System.currentTimeMillis();
                faces = sFaceDetector.detect(bitmap);
                final long endTime = System.currentTimeMillis();
                sRecognitionTime = endTime - startTime;
                sRecognitionTimeSum += sRecognitionTime;
                sFrameCount++;
                if (faces != null && faces.size() > 0) {
                    sRecognizedFrameCount++;
                }

                activity.runOnUiThread(
                        new Runnable() {
                            @SuppressLint("StringFormatMatches")
                            @Override
                            public void run() {
                                long percent = 0;
                                long average = 0;
                                if (sFrameCount != 0) {
                                    percent = sRecognizedFrameCount * 100 / sFrameCount;
                                    average = sRecognitionTimeSum / sFrameCount;
                                }
                                score.setText(activity.getResources().getString(
                                        R.string.face_recognition_time,
                                        sRecognitionTime, average, percent));
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
        if (faces != null && faces.size() > 0) {
            // Draw one face only on preview bitmap
            Rect face = faces.get(0);
            Canvas canvas = new Canvas(bitmap);
            float scale = (float) bitmap.getHeight() / 192;
            if (scale < 1) {
                scale = 1.0f;
            }
            // Draw box
            sFaceLandmarkPaint.setColor(Color.RED);
            sFaceLandmarkPaint.setStrokeWidth(scale);
            canvas.drawRect(face, sFaceLandmarkPaint);
        }
        if (showMask) {
            faceView.setPoseAnglesAndModelView(false, null, null, null, null);
        }
    }
}
