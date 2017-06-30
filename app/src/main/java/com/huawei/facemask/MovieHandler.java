package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.utils.MediaUtils;

import java.io.File;
import java.io.FilenameFilter;

import static com.huawei.facemask.OnGetImageListener.DLIB_FACE_RECOGNITION;
import static com.huawei.facemask.OnGetImageListener.GMS_FACE_RECOGNITION;

class MovieHandler {

    private static final String TAG = "MovieHandler";
    private static File[] sVideoFiles;
    private static int sFaceRecognition = DLIB_FACE_RECOGNITION;
    private static boolean sRunning;
    private static long sLastElapsedTime = 0;
    private static float mLastFrameRate = 0;
    private static boolean sExtracting;
    private static float sBrightness;
    private static float sContrast;
    private static Handler sInferenceHandler;
    private static int sVideoWidth;
    private static int sVideoHeight;
    private static float sFrameRate;

    // Declare this here to reduce allocations.
    private static MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();

    static boolean init(Context context, FloatingPreviewWindow preview, Handler handler) {
        sRunning = false;
        boolean result = false;
        sLastElapsedTime = 0;
        sInferenceHandler = handler;
        File folder = Environment.getExternalStorageDirectory();
        folder = new File(folder, "Facemask/Movies");
        if (folder.exists()) {
            sVideoFiles = folder.listFiles(new FilenameFilter() {
                @Override
                public boolean accept(File dir, String name) {
                    return MediaUtils.isVideoFile(name);
                }
            });
            if (sVideoFiles.length > 0) {
                File videoFile = sVideoFiles[0];
                Bitmap bitmap = MediaUtils.createMovieThumbnail(videoFile.getAbsolutePath());
                preview.setRGBBitmap(bitmap);
                result = true;
            } else {
                Toast.makeText(context, context.getString(R.string.folder_movies_is_empty),
                        Toast.LENGTH_LONG).show();
            }
        } else {
            Toast.makeText(context, context.getString(R.string.folder_movies_not_found),
                    Toast.LENGTH_LONG).show();
        }
        return result;
    }

    public static boolean isRunning() {
        return sRunning;
    }

    public static void stop() {
        sRunning = false;
    }

    static void setBrightness(float brightness) {
        sBrightness = brightness;
    }

    static void setContrast(float contrast) {
        sContrast = contrast;
    }

    static void setFaceRecognition(int feature) {
        sFaceRecognition = feature;
    }


    private static Bitmap imageSideInversion(Bitmap src) {
        Log.d(TAG, "sVideoWidth = " + sVideoWidth);
        Log.d(TAG, "sVideoHeight = " + sVideoHeight);
        Log.d(TAG, "src.getWidth() = " + src.getWidth());
        Log.d(TAG, "src.getHeight() = " + src.getHeight());
        final Matrix matrix = new Matrix();
        final RectF viewRect = new RectF(0, 0, src.getWidth(), src.getHeight());
//        final RectF bufferRect = new RectF(0, 0, sVideoHeight, sVideoWidth);
        final float centerX = viewRect.centerX();
        final float centerY = viewRect.centerY();
//        bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
//        matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
        matrix.setScale(0.3f * (float) src.getHeight() / src.getWidth(),
                0.3f * (float) src.getWidth() / src.getHeight(), centerX, centerY);
        //matrix.postRotate(90, centerX, centerY);
        Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), matrix, false);
        Bitmap outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(outBmp);
        ColorMatrix cm = new ColorMatrix(new float[]{
                sContrast, 0, 0, 0, sBrightness,
                0, sContrast, 0, 0, sBrightness,
                0, 0, sContrast, 0, sBrightness,
                0, 0, 0, 1, 0
        });
        Paint paint = new Paint();
        paint.setColorFilter(new ColorMatrixColorFilter(cm));
        canvas.drawBitmap(inBmp, 0, 0, paint);
        return outBmp;
    }

/*
    private static Bitmap imageSideInversion(Bitmap src) {
        Matrix sideInversion = new Matrix();
        sideInversion.setScale(0.3f, 0.3f);
        sideInversion.setRotate(90);
        Log.d(TAG, "sVideoWidth = " + sVideoWidth);
        Log.d(TAG, "sVideoHeight = " + sVideoHeight);
        Log.d(TAG, "src.getWidth() = " + src.getWidth());
        Log.d(TAG, "src.getHeight() = " + src.getHeight());

        Bitmap inBmp = Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), sideInversion, false);
        Bitmap outBmp = Bitmap.createBitmap(inBmp.getWidth(), inBmp.getHeight(), Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(outBmp);
        ColorMatrix cm = new ColorMatrix(new float[]{
                sContrast, 0, 0, 0, sBrightness,
                0, sContrast, 0, 0, sBrightness,
                0, 0, sContrast, 0, sBrightness,
                0, 0, 0, 1, 0
        });
        Paint paint = new Paint();
        paint.setColorFilter(new ColorMatrixColorFilter(cm));
        canvas.drawBitmap(inBmp, 0, 0, paint);
        return outBmp;
    }
*/

    static void processImage(final Bitmap bitmap, final Activity activity, final TextureView texture,
                             final TextView score, TextView frameRateView, final TextView mouthOpen,
                             final FloatingPreviewWindow preview) {

        long elapsedTime = SystemClock.elapsedRealtime();

        if (sLastElapsedTime != 0) {
            float frameRate = 1000f / (elapsedTime - sLastElapsedTime);
            if (mLastFrameRate == 0) {
                mLastFrameRate = frameRate;
            } else {
                mLastFrameRate = ((mLastFrameRate * 29) + frameRate) / 30;
            }
            frameRateView.setText(activity.getResources().getString(
                    R.string.camera_preview_frame_rate, mLastFrameRate) + " (" + sFrameRate + ")");
        }
        sLastElapsedTime = elapsedTime;

        if (sExtracting) {
            return;
        }
        sExtracting = true;
        sInferenceHandler.post(
                new Runnable() {
                    @Override
                    public void run() {
                        Bitmap out = imageSideInversion(bitmap);
                        switch (sFaceRecognition) {
                            case DLIB_FACE_RECOGNITION:
                                Dlib.detectFace(activity, score, mouthOpen, out);
                                break;
                            case GMS_FACE_RECOGNITION:
                                GmsVision.detectFace(activity, score, out);
                                break;
                        }
                        preview.setRGBBitmap(out);
                        sExtracting = false;
                    }
                });
    }


    public static void start(final Activity activity, final TextureView texture, final TextView score,
                             final TextView mouthOpen, final FloatingPreviewWindow preview) {
        AsyncActionHandler.post(new Runnable() {
            @Override
            public void run() {
                if (sVideoFiles == null || sVideoFiles.length == 0) {
                    return;
                }
                int index = 0;
                sRunning = true;
                SurfaceTexture st = texture.getSurfaceTexture();
                texture.setDrawingCacheEnabled(true);
                Surface sf = new Surface(st);
                File movie = sVideoFiles[index];
                MediaPlayer player = MediaPlayer.create(activity, Uri.fromFile(movie));
                player.setLooping(true);
                player.setSurface(sf);
                sVideoHeight = player.getVideoHeight();
                sVideoWidth = player.getVideoWidth();

                while (sRunning) {
                    player.start();
                }
                player.release();
                sf.release();
            }
        });
    }
}
