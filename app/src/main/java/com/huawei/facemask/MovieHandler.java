package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.view.Surface;
import android.view.TextureView;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.utils.MediaUtils;

import java.io.File;
import java.io.FilenameFilter;

import static com.huawei.facemask.OnGetImageListener.DLIB_FACE_RECOGNITION;

class MovieHandler {

    private static final String TAG = "MovieHandler";
    private static File[] sVideoFiles;
    private static int sFaceRecognition = DLIB_FACE_RECOGNITION;
    private static boolean sRunning;
    private static boolean sExtracting;
    private static float sBrightness;
    private static float sContrast;

    // Declare this here to reduce allocations.
    private static MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();

    static boolean init(Context context, FloatingPreviewWindow preview) {
        sRunning = false;
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

                while (sRunning) {
                    player.start();
                }
                player.release();
                sf.release();
            }
        });
    }
}
