package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.SparseArray;
import android.widget.TextView;

import com.google.android.gms.vision.Detector;
import com.google.android.gms.vision.Frame;
import com.google.android.gms.vision.face.Face;
import com.google.android.gms.vision.face.FaceDetector;
import com.google.android.gms.vision.face.Landmark;
import com.huawei.gmsvision.GmsFaceDetector;

class GmsVision {

    private static final Paint sFaceLandmarkPaint = new Paint();
    private static Detector<Face> sFaceDetector;
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
        if (sFaceDetector != null) {
            sFaceDetector.release();
        }
        if (detectLandmarks) {
            FaceDetector detector = new FaceDetector.Builder(context)
                    .setTrackingEnabled(false)
                    .setProminentFaceOnly(true)
                    .setLandmarkType(FaceDetector.ALL_LANDMARKS)
                    .build();
            sFaceDetector = new GmsFaceDetector(detector);
        } else {
            FaceDetector detector = new FaceDetector.Builder(context)
                    .setTrackingEnabled(false)
                    .setProminentFaceOnly(true)
                    .setLandmarkType(FaceDetector.NO_LANDMARKS)
                    .build();
            sFaceDetector = new GmsFaceDetector(detector);
        }
    }

    static void detectFace(final Activity activity, final TextView score, Bitmap bitmap) {
        SparseArray<Face> faces = null;
        if (sFaceDetector.isOperational()) {
            final long startTime = System.currentTimeMillis();
            Frame frame = new Frame.Builder().setBitmap(bitmap).build();
            faces = sFaceDetector.detect(frame);
            final long endTime = System.currentTimeMillis();
            sRecognitionTime = endTime - startTime;
            sRecognitionTimeSum += sRecognitionTime;
            sFrameCount++;
            if (faces != null && faces.size() > 0) {
                sRecognizedFrameCount++;
            }

            activity.runOnUiThread(
                    new Runnable() {
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
        if (faces != null && faces.size() > 0) {
            // Draw one face only on preview bitmap
            Face face = faces.valueAt(0);
            Canvas canvas = new Canvas(bitmap);
            float scale = (float) bitmap.getHeight() / 192;
            // Draw box
            sFaceLandmarkPaint.setColor(Color.RED);
            sFaceLandmarkPaint.setStrokeWidth(scale);
            canvas.drawRect(new Rect((int) face.getPosition().x,
                    (int) face.getPosition().y,
                    (int) (face.getPosition().x + face.getWidth()),
                    (int) (face.getPosition().y + face.getHeight())), sFaceLandmarkPaint);
            // Draw landmarks
            sFaceLandmarkPaint.setColor(Color.WHITE);
            for (Landmark landmark : face.getLandmarks()) {
                int cx = (int) (landmark.getPosition().x);
                int cy = (int) (landmark.getPosition().y);
                canvas.drawCircle(cx, cy, 1.5f * scale, sFaceLandmarkPaint);
            }
        }
    }

}
