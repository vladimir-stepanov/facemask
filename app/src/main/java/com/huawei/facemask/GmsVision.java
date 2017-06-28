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

import static com.huawei.facemask.OnGetImageListener.SCALE;

class GmsVision {

    private static final Paint sFaceLandmarkPaint = new Paint();
    private static Detector<Face> sFaceDetector;

    static {
        sFaceLandmarkPaint.setColor(Color.WHITE);
        sFaceLandmarkPaint.setStyle(Paint.Style.STROKE);
        sFaceLandmarkPaint.setStrokeWidth(2f * SCALE);
    }

    static void release() {
        if (sFaceDetector != null) {
            sFaceDetector.release();
        }
    }

    static void setLandmarksDetection(Context context, boolean detectLandmarks) {
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

    static boolean detectFace(final Activity activity, final TextView score, Bitmap bitmap) {
        SparseArray<Face> faces = null;
        boolean result = false;
        if (sFaceDetector.isOperational()) {
            final long startTime = System.currentTimeMillis();
            Frame frame = new Frame.Builder().setBitmap(bitmap).build();
            faces = sFaceDetector.detect(frame);
            final long endTime = System.currentTimeMillis();
            activity.runOnUiThread(
                    new Runnable() {
                        @Override
                        public void run() {
                            score.setText(activity.getResources().getString(
                                    R.string.face_recognition_time, endTime - startTime));
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
            result = true;
            Face face = faces.valueAt(0);
            Canvas canvas = new Canvas(bitmap);
            // Draw box
            sFaceLandmarkPaint.setColor(Color.DKGRAY);
            canvas.drawRect(new Rect((int) face.getPosition().x,
                    (int) face.getPosition().y,
                    (int) (face.getPosition().x + face.getWidth()),
                    (int) (face.getPosition().y + face.getHeight())), sFaceLandmarkPaint);
            // Draw landmarks
            sFaceLandmarkPaint.setColor(Color.WHITE);
            for (Landmark landmark : face.getLandmarks()) {
                int cx = (int) (landmark.getPosition().x);
                int cy = (int) (landmark.getPosition().y);
                canvas.drawCircle(cx, cy, 3.0f * SCALE, sFaceLandmarkPaint);
            }
        }
        return result;
    }

}
