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
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.SystemClock;
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

    static float scale = 0.5f;
    private static File[] sVideoFiles;
    private static int sFaceRecognition = DLIB_FACE_RECOGNITION;
    private static boolean sRunning;
    private static long sLastElapsedTime = 0;
    private static float sLastFrameRate = 0;
    private static long sFrameCount;
    private static float sFrameSum;
    private static float sAverageFrameRate;
    private static boolean sExtracting;
    private static float sBrightness;
    private static float sContrast;
    private static Handler sInferenceHandler;
    private static int sVideoWidth;
    private static int sVideoHeight;

    static boolean haveMovies(Context context) {
        boolean result = false;
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

    static void init(Context context, FloatingPreviewWindow preview, Handler handler) {
        clearStatistics();
        sRunning = false;
        sInferenceHandler = handler;
        File movie = sVideoFiles[0];
        MediaPlayer player = MediaPlayer.create(context, Uri.fromFile(movie));
        sVideoHeight = player.getVideoHeight();
        sVideoWidth = player.getVideoWidth();
        Bitmap bitmap = MediaUtils.createMovieThumbnail(movie.getAbsolutePath());
        preview.setRGBBitmap(bitmap);
        player.release();
    }

    public static boolean isRunning() {
        return sRunning;
    }

    public static void stop() {
        boolean delay = sRunning;
        sRunning = false;
        if (delay) {
            try {
                Thread.sleep(200);
            } catch (InterruptedException ignored) {
            }
        }
    }

    static void clearStatistics() {
        sLastElapsedTime = 0;
        sLastFrameRate = 0;
        sFrameCount = 0;
        sFrameSum = 0;
        Dlib.clearStatistics();
        GmsVision.clearStatistics();
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

    static float getVideoWidth() {
        return sVideoWidth;
    }

    static float getVideoHeight() {
        return sVideoHeight;
    }

    private static Bitmap preSideInversion(Bitmap src) {
        final Matrix matrix = new Matrix();
        if (sVideoWidth > sVideoHeight) {
            final RectF viewRect = new RectF(0, 0, src.getWidth(), src.getHeight());
            final float centerX = viewRect.centerX();
            final float centerY = viewRect.centerY();
            matrix.setRotate(-90, centerX, centerY);
            matrix.setScale(scale * (float) src.getHeight() / src.getWidth(),
                    scale * (float) src.getWidth() / src.getHeight(), centerX, centerY);
        } else {
            matrix.setScale(scale, scale);
        }
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

    private static Bitmap postSideInversion(Bitmap src) {
        final Matrix matrix = new Matrix();
        if (sVideoWidth > sVideoHeight) {
            final RectF viewRect = new RectF(0, 0, src.getWidth(), src.getHeight());
            final float centerX = viewRect.centerX();
            final float centerY = viewRect.centerY();
            matrix.setRotate(90, centerX, centerY);
        }
        return Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), matrix, false);
    }

    static void processImage(final Activity activity, final TextureView texture,
                             final TextView score, TextView frameRateView, final TextView mouthOpen,
                             final FloatingPreviewWindow preview) {

        long elapsedTime = SystemClock.elapsedRealtime();

        if (sLastElapsedTime != 0) {
            float frameRate = 1000f / (elapsedTime - sLastElapsedTime);
            if (sLastFrameRate == 0) {
                sLastFrameRate = frameRate;
            } else {
                sFrameSum += frameRate;
                sFrameCount++;
                sAverageFrameRate = sFrameSum / sFrameCount;
                sLastFrameRate = (sLastFrameRate + frameRate) / 2;
            }
            frameRateView.setText(activity.getString(
                    R.string.camera_preview_frame_rate, sLastFrameRate, sAverageFrameRate));
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
                        Bitmap src = texture.getBitmap();
                        Bitmap out = preSideInversion(src);
                        switch (sFaceRecognition) {
                            case DLIB_FACE_RECOGNITION:
                                Dlib.detectFace(activity, score, mouthOpen, out);
                                break;
                            case GMS_FACE_RECOGNITION:
                                GmsVision.detectFace(activity, score, out);
                                break;
                        }
                        if (sRunning) {
                            preview.setRGBBitmap(postSideInversion(out));
                        }
                        sExtracting = false;
                    }
                });
    }

    public static void start(final Activity activity, final TextureView texture) {
        clearStatistics();
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
                player.setVolume(0f, 0f);
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
