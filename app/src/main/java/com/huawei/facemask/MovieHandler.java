package com.huawei.facemask;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.utils.MediaUtils;

import java.io.File;
import java.io.FilenameFilter;
import java.nio.ByteBuffer;

import static com.huawei.facemask.OnGetImageListener.DLIB_FACE_RECOGNITION;

class MovieHandler {

    private static final boolean VERBOSE = false;
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

    /**
     * Selects the video track, if any.
     *
     * @return the track index, or -1 if no video track is found.
     */
    private static int selectTrack(MediaExtractor extractor) {
        // Select the first video track we find, ignore the rest.
        int numTracks = extractor.getTrackCount();
        for (int i = 0; i < numTracks; i++) {
            MediaFormat format = extractor.getTrackFormat(i);
            String mime = format.getString(MediaFormat.KEY_MIME);
            if (mime.startsWith("video/")) {
                Log.d(TAG, "Extractor selected track " + i + " (" + mime + "): " + format);
                return i;
            }
        }

        return -1;
    }

    /**
     * Work loop.  We execute here until we run out of video or are told to stop.
     */
    private static void doExtract(MediaExtractor extractor, int trackIndex, MediaCodec decoder) {
        // We need to strike a balance between providing input and reading output that
        // operates efficiently without delays on the output side.
        //
        // To avoid delays on the output side, we need to keep the codec's input buffers
        // fed.  There can be significant latency between submitting frame N to the decoder
        // and receiving frame N on the output, so we need to stay ahead of the game.
        //
        // Many video decoders seem to want several frames of video before they start
        // producing output -- one implementation wanted four before it appeared to
        // configure itself.  We need to provide a bunch of input frames up front, and try
        // to keep the queue full as we go.
        //
        // (Note it's possible for the encoded data to be written to the stream out of order,
        // so we can't generally submit a single frame and wait for it to appear.)
        //
        // We can't just fixate on the input side though.  If we spend too much time trying
        // to stuff the input, we might miss a presentation deadline.  At 60Hz we have 16.7ms
        // between frames, so sleeping for 10ms would eat up a significant fraction of the
        // time allowed.  (Most video is at 30Hz or less, so for most content we'll have
        // significantly longer.)  Waiting for output is okay, but sleeping on availability
        // of input buffers is unwise if we need to be providing output on a regular schedule.
        //
        //
        // In some situations, startup latency may be a concern.  To minimize startup time,
        // we'd want to stuff the input full as quickly as possible.  This turns out to be
        // somewhat complicated, as the codec may still be starting up and will refuse to
        // accept input.  Removing the timeout from dequeueInputBuffer() results in spinning
        // on the CPU.
        //
        // If you have tight startup latency requirements, it would probably be best to
        // "prime the pump" with a sequence of frames that aren't actually shown (e.g.
        // grab the first 10 NAL units and shove them through, then rewind to the start of
        // the first key frame).
        //
        // The actual latency seems to depend on strongly on the nature of the video (e.g.
        // resolution).
        //
        //
        // One conceptually nice approach is to loop on the input side to ensure that the codec
        // always has all the input it can handle.  After submitting a buffer, we immediately
        // check to see if it will accept another.  We can use a short timeout so we don't
        // miss a presentation deadline.  On the output side we only check once, with a longer
        // timeout, then return to the outer loop to see if the codec is hungry for more input.
        //
        // In practice, every call to check for available buffers involves a lot of message-
        // passing between threads and processes.  Setting a very brief timeout doesn't
        // exactly work because the overhead required to determine that no buffer is available
        // is substantial.  On one device, the "clever" approach caused significantly greater
        // and more highly variable startup latency.
        //
        // The code below takes a very simple-minded approach that works, but carries a risk
        // of occasionally running out of output.  A more sophisticated approach might
        // detect an output timeout and use that as a signal to try to enqueue several input
        // buffers on the next iteration.
        //
        // If you want to experiment, set the VERBOSE flag to true and watch the behavior
        // in logcat.  Use "logcat -v threadtime" to see sub-second timing.

        final int TIMEOUT_USEC = 10000;
        ByteBuffer[] decoderInputBuffers = decoder.getInputBuffers();
        int inputChunk = 0;
        long firstInputTimeNsec = -1;

        boolean outputDone = false;
        boolean inputDone = false;
        while (!outputDone) {
            if (VERBOSE) Log.d(TAG, "loop");
            if (!sRunning) {
                Log.d(TAG, "Stop requested");
                return;
            }

            // Feed more data to the decoder.
            if (!inputDone) {
                int inputBufIndex = decoder.dequeueInputBuffer(TIMEOUT_USEC);
                if (inputBufIndex >= 0) {
                    if (firstInputTimeNsec == -1) {
                        firstInputTimeNsec = System.nanoTime();
                    }
                    ByteBuffer inputBuf = decoderInputBuffers[inputBufIndex];
                    // Read the sample data into the ByteBuffer.  This neither respects nor
                    // updates inputBuf's position, limit, etc.
                    int chunkSize = extractor.readSampleData(inputBuf, 0);
                    if (chunkSize < 0) {
                        // End of stream -- send empty frame with EOS flag set.
                        decoder.queueInputBuffer(inputBufIndex, 0, 0, 0L,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                        inputDone = true;
                        if (VERBOSE) Log.d(TAG, "sent input EOS");
                    } else {
                        if (extractor.getSampleTrackIndex() != trackIndex) {
                            Log.w(TAG, "WEIRD: got sample from track " +
                                    extractor.getSampleTrackIndex() + ", expected " + trackIndex);
                        }
                        long presentationTimeUs = extractor.getSampleTime();
                        decoder.queueInputBuffer(inputBufIndex, 0, chunkSize,
                                presentationTimeUs, 0 /*flags*/);
                        if (VERBOSE) {
                            Log.d(TAG, "submitted frame " + inputChunk + " to dec, size=" +
                                    chunkSize);
                        }
                        inputChunk++;
                        extractor.advance();
                    }
                } else {
                    if (VERBOSE) Log.d(TAG, "input buffer not available");
                }
            }

            if (!outputDone) {
                int decoderStatus = decoder.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
                if (decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    // no output available yet
                    if (VERBOSE) Log.d(TAG, "no output from decoder available");
                } else if (decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                    // not important for us, since we're using Surface
                    if (VERBOSE) Log.d(TAG, "decoder output buffers changed");
                } else if (decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    MediaFormat newFormat = decoder.getOutputFormat();
                    if (VERBOSE) Log.d(TAG, "decoder output format changed: " + newFormat);
                } else if (decoderStatus < 0) {
                    throw new RuntimeException(
                            "unexpected result from decoder.dequeueOutputBuffer: " +
                                    decoderStatus);
                } else { // decoderStatus >= 0
                    if (firstInputTimeNsec != 0) {
                        // Log the delay from the first buffer of input to the first buffer
                        // of output.
                        long nowNsec = System.nanoTime();
                        Log.d(TAG, "startup lag " + ((nowNsec - firstInputTimeNsec) / 1000000.0) + " ms");
                        firstInputTimeNsec = 0;
                    }
                    boolean doLoop = false;
                    if (VERBOSE) Log.d(TAG, "surface decoder given buffer " + decoderStatus +
                            " (size=" + mBufferInfo.size + ")");
                    if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        if (VERBOSE) Log.d(TAG, "output EOS");
                        doLoop = true;
                    }

                    boolean doRender = (mBufferInfo.size != 0);

                    // As soon as we call releaseOutputBuffer, the buffer will be forwarded
                    // to SurfaceTexture to convert to a texture.  We can't control when it
                    // appears on-screen, but we can manage the pace at which we release
                    // the buffers.
                    decoder.releaseOutputBuffer(decoderStatus, doRender);

                    if (doLoop) {
                        Log.d(TAG, "Reached EOS, looping");
                        extractor.seekTo(0, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
                        inputDone = false;
                        decoder.flush();    // reset decoder state
                    }
                }
            }
        }
    }

    private SurfaceTexture mSurfaceTexture;


    private static void processBitmap(TextureView texture, FloatingPreviewWindow preview) {
        if (sExtracting) {
            return;
        }
        sExtracting = true;
        Bitmap bitmap = texture.getBitmap();
        preview.setRGBBitmap(bitmap);
        sExtracting = false;
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
                sExtracting = false;
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
