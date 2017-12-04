package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.os.Environment;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.opencv.ObjTracker;
import com.huawei.utils.MediaUtils;

import java.io.File;
import java.io.FilenameFilter;

import grapevine.face1.FaceView;

import static com.huawei.facemask.OnGetImageListener.BOOSTING_FACE_TRACKER;
import static com.huawei.facemask.OnGetImageListener.DLIB_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.DLIB_MOD_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.GMS_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.HAAR_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.KCF_FACE_TRACKER;
import static com.huawei.facemask.OnGetImageListener.LBP_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.SEETAFACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.MEDIANFLOW_FACE_TRACKER;
import static com.huawei.facemask.OnGetImageListener.MIL_FACE_TRACKER;
import static com.huawei.facemask.OnGetImageListener.TLD_FACE_TRACKER;

class ImageHandler {

    private static File[] sImageFiles;
    private static int sFaceRecognition = DLIB_FACE_RECOGNITION;
    private static boolean sRunning;
    private static float sBrightness;
    private static float sContrast;
    private static boolean sShowMask;

    static boolean haveImages(Context context) {
        boolean result = false;
        File folder = Environment.getExternalStorageDirectory();
        folder = new File(folder, "Facemask/Images");
        if (folder.exists()) {
            sImageFiles = folder.listFiles(new FilenameFilter() {
                @Override
                public boolean accept(File dir, String name) {
                    return MediaUtils.isImageFile(name);
                }
            });
            if (sImageFiles.length > 0) {
                result = true;
            } else {
                Toast.makeText(context, context.getString(R.string.folder_images_is_empty),
                        Toast.LENGTH_LONG).show();
            }
        } else {
            Toast.makeText(context, context.getString(R.string.folder_images_not_found),
                    Toast.LENGTH_LONG).show();
        }
        return result;
    }

    static void clearStatistics() {
        Dlib.clearStatistics();
        DlibMod.clearStatistics();
        GmsVision.clearStatistics();
        HaarCascade.clearStatistics();
        LbpCascade.clearStatistics();
        Seetaface.clearStatistics();
        FaceTracker.clearStatistics();
    }

    static void init(FloatingPreviewWindow preview) {
        clearStatistics();
        sRunning = false;
        File image = sImageFiles[0];
        Bitmap bitmap = MediaUtils.createImageThumbnail(image.getAbsolutePath());
        preview.setRGBBitmap(bitmap);
    }

    public static boolean isRunning() {
        return sRunning;
    }

    public static void stop() {
        sRunning = false;
    }


    static void setShowMask(boolean showMask) {
        sShowMask = showMask;
    }

    static void setBrightness(float brightness) {
        clearStatistics();
        sBrightness = brightness;
    }

    static void setContrast(float contrast) {
        clearStatistics();
        sContrast = contrast;
    }

    static void setFaceRecognition(int feature) {
        clearStatistics();
        sFaceRecognition = feature;
    }

    public static void start(final Activity activity, final TextView score, final FaceView faceView,
                             final TextView mouthOpen, final FloatingPreviewWindow preview,
                             final ImageView mMediaPlayPauseButton) {
        clearStatistics();
        AsyncActionHandler.post(new Runnable() {
            @Override
            public void run() {
                if (sImageFiles == null || sImageFiles.length == 0) {
                    return;
                }
                int index = 0;
                sRunning = true;
                while (sRunning) {
                    File image = sImageFiles[index];
                    // Process image
                    Bitmap srcBmp = MediaUtils.createImageThumbnail(image.getAbsolutePath());
                    if (srcBmp != null) {
                        // contrast = 0..10 1 is default
                        // brightness = -255..255 0 is default
                        Bitmap dstBmp = Bitmap.createBitmap(srcBmp.getWidth(), srcBmp.getHeight(), Bitmap.Config.ARGB_8888);
                        Canvas canvas = new Canvas(dstBmp);
                        ColorMatrix cm = new ColorMatrix(new float[]{
                                sContrast, 0, 0, 0, sBrightness,
                                0, sContrast, 0, 0, sBrightness,
                                0, 0, sContrast, 0, sBrightness,
                                0, 0, 0, 1, 0
                        });
                        Paint paint = new Paint();
                        paint.setColorFilter(new ColorMatrixColorFilter(cm));
                        canvas.drawBitmap(srcBmp, 0, 0, paint);

                        switch (sFaceRecognition) {
                            case DLIB_FACE_RECOGNITION:
                                Dlib.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case DLIB_MOD_FACE_RECOGNITION:
                                DlibMod.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case GMS_FACE_RECOGNITION:
                                GmsVision.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case SEETAFACE_RECOGNITION:
                                Seetaface.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case HAAR_FACE_RECOGNITION:
                                HaarCascade.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case LBP_FACE_RECOGNITION:
                                LbpCascade.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask);
                                break;
                            case MIL_FACE_TRACKER:
                                //FaceTracker.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask, ObjTracker.MIL_TRACKER_ALGORITHM);
                                Toast.makeText(activity.getApplicationContext(), "TRACKERS FEATURE NOT SUPPORTED!!!",
                                        Toast.LENGTH_LONG).show();
                                break;
                            case BOOSTING_FACE_TRACKER:
                                //FaceTracker.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask, ObjTracker.BOOSTING_TRACKER_ALGORITHM);
                                break;
                            case TLD_FACE_TRACKER:
                                //FaceTracker.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask, ObjTracker.TLD_TRACKER_ALGORITHM);
                                break;
                            case MEDIANFLOW_FACE_TRACKER:
                                //FaceTracker.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask, ObjTracker.MEDIANFLOW_TRACKER_ALGORITHM);
                                break;
                            case KCF_FACE_TRACKER:
                                //FaceTracker.detectFace(activity, faceView, score, mouthOpen, dstBmp, sShowMask, ObjTracker.KCF_TRACKER_ALGORITHM);
                                 break;
                        }
                        preview.setRGBBitmap(dstBmp);
                    }
                    index++;
                    if (index >= sImageFiles.length) {
                        stop();
                    }
                }
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_play);
                    }
                });
            }
        });
    }
}
