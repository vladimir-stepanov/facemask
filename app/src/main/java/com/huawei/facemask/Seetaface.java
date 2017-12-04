package com.huawei.facemask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.widget.TextView;

import com.huawei.dlib.DlibDetectedFace;
import com.huawei.seetaface.SeetafaceDetector;

import java.util.ArrayList;
import java.util.List;

class Seetaface {

    private static final Paint sFaceLandmarkPaint = new Paint();
    @SuppressLint("StaticFieldLeak")
    private static SeetafaceDetector sFaceDetector;
    private static boolean sDetectLandmarks;
    private static long sRecognitionTime;
    private static float sRecognitionTimeSum;
    private static long sNativeRecognitionTime;
    private static float sNativeRecognitionTimeSum;
    private static long sRecognizedFrameCount;
    private static long sFrameCount;

    static {
        sFaceLandmarkPaint.setColor(Color.WHITE);
        sFaceLandmarkPaint.setStyle(Paint.Style.STROKE);
    }

    static void clearStatistics() {
        sRecognitionTime = 0;
        sRecognitionTimeSum = 0;
        sNativeRecognitionTime = 0;
        sNativeRecognitionTimeSum = 0;
        sRecognizedFrameCount = 0;
        sFrameCount = 0;
    }

    static void setLandmarksDetection(Context context, boolean detectLandmarks) {
        clearStatistics();
        sFaceDetector = SeetafaceDetector.getInstance(context);
        sDetectLandmarks = detectLandmarks;
        sFaceDetector.mDetectLandmarks = detectLandmarks ? 1 : 0;
    }

    static void release() {
        if (sFaceDetector != null) {
            sFaceDetector.release();
        }
    }

    static void detectFace(final Activity activity, final TextView score,
                           final TextView mouth, Bitmap bitmap) {
        List<DlibDetectedFace> faces = null;
        if (sFaceDetector != null) {
            if (sFaceDetector.isInitiated()) {
                final long startTime = System.currentTimeMillis();
                faces = sFaceDetector.detect(bitmap);
                final long endTime = System.currentTimeMillis();
                sRecognitionTime = endTime - startTime;
                sRecognitionTimeSum += sRecognitionTime;
                sNativeRecognitionTime = sFaceDetector.mSpentTime;
                sNativeRecognitionTimeSum += sNativeRecognitionTime;
                sFrameCount++;
                if (faces != null && faces.size() > 0) {
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
                                        R.string.face_recognition_time,
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
        if (faces != null && faces.size() > 0) {
            // Draw one face only on preview bitmap
            DlibDetectedFace face = faces.get(0);
            Canvas canvas = new Canvas(bitmap);
            float scale = (float) bitmap.getHeight() / 192;
            if (scale < 1) {
                scale = 1.0f;
            }
            // Draw box
            sFaceLandmarkPaint.setColor(Color.RED);
            sFaceLandmarkPaint.setStrokeWidth(scale);
            canvas.drawRect(face.getBounds(), sFaceLandmarkPaint);
            if (sDetectLandmarks) {
                // Draw landmarks
                sFaceLandmarkPaint.setColor(Color.WHITE);
                ArrayList<Point> landmarks = face.getFaceLandmarks();
                for (Point point : landmarks) {
                    canvas.drawCircle(point.x, point.y, scale / 2, sFaceLandmarkPaint);
                }
                if (!landmarks.isEmpty()) {
                    // draw a line sticking out of the nose
                    sFaceLandmarkPaint.setColor(Color.BLUE);
                    Point start = landmarks.get(2);//face.getNoseStart();
                    Point stop = face.getNoseEnd();
                    canvas.drawLine(start.x, start.y, stop.x, stop.y, sFaceLandmarkPaint);
                }
            }
            final float mouthOpen = face.getMouthOpen();
            activity.runOnUiThread(
                    new Runnable() {
                        @Override
                        public void run() {
                            mouth.setText(activity.getResources().getString(
                                    R.string.mouth_open, mouthOpen));
                        }
                    });
        } else {
            activity.runOnUiThread(
                    new Runnable() {
                        @Override
                        public void run() {
                            mouth.setText(activity.getResources().getString(
                                    R.string.mouth_open, 0f));
                        }
                    });
        }
    }

}
