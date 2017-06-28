package com.huawei.facemask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.widget.TextView;

import com.huawei.dlib.DlibDetectedFace;
import com.huawei.dlib.DlibFaceDetector;

import java.util.ArrayList;
import java.util.List;

import static com.huawei.facemask.OnGetImageListener.SCALE;

class Dlib {

    private static final Paint sFaceLandmarkPaint = new Paint();
    @SuppressLint("StaticFieldLeak")
    private static final DlibFaceDetector sFaceDetector = DlibFaceDetector.getInstance();
    private static boolean sDetectLandmarks;

    static {
        sFaceLandmarkPaint.setColor(Color.WHITE);
        sFaceLandmarkPaint.setStyle(Paint.Style.STROKE);
        sFaceLandmarkPaint.setStrokeWidth(2f * SCALE);
    }

    static void setLandmarksDetection(boolean detectLandmarks) {
        sDetectLandmarks = detectLandmarks;
    }

    static void release() {
        if (sFaceDetector != null) {
            sFaceDetector.release();
        }
    }

    static boolean detectFace(final Activity activity, final TextView score,
                              final TextView mouth, Bitmap bitmap) {
        boolean result = false;
        List<DlibDetectedFace> faces = null;
        if (sFaceDetector != null) {
            if (sFaceDetector.isInitiated()) {
                final long startTime = System.currentTimeMillis();
                faces = sFaceDetector.detect(bitmap);
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
        }
        if (faces != null && faces.size() > 0) {
            result = true;
            // Draw one face only on preview bitmap
            DlibDetectedFace face = faces.get(0);
            Canvas canvas = new Canvas(bitmap);
            // Draw box
            sFaceLandmarkPaint.setColor(Color.DKGRAY);
            canvas.drawRect(face.getBounds(), sFaceLandmarkPaint);
            if (sDetectLandmarks) {
                // Draw landmarks
                sFaceLandmarkPaint.setColor(Color.WHITE);
                ArrayList<Point> landmarks = face.getFaceLandmarks();
                for (Point point : landmarks) {
                    canvas.drawCircle(point.x, point.y, 3.0f * SCALE, sFaceLandmarkPaint);
                }
                // draw a line sticking out of the nose
                sFaceLandmarkPaint.setColor(Color.BLUE);
                Point start = face.getNoseStart();
                Point stop = face.getNoseEnd();
                canvas.drawLine(start.x, start.y, stop.x, stop.y, sFaceLandmarkPaint);
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

/*
            // Send face to grapevine
            mFaceView.setPoseAnglesAndModelView(true,
                    face.mAngles,
                    face.mModelView,
                    face.mFrustumScale,
                    face.mLandmarks);
*/
        } else {
            //mFaceView.setPoseAnglesAndModelView(false, null, null, null, null);
            activity.runOnUiThread(
                    new Runnable() {
                        @Override
                        public void run() {
                            mouth.setText(activity.getResources().getString(
                                    R.string.mouth_open, 0f));
                        }
                    });
        }
        return result;
    }

}
