package com.hfs.furyclient.activities;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Size;
import android.view.Display;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.hfs.furyclient.R;
import com.hfs.furyclient.data.HFSInstruction;
import com.hfs.furyclient.opencv.HFSObjTracker;
import com.hfs.furyclient.utils.DetectedObject;
import com.hfs.furyclient.utils.HFSObjectTracker;
import com.hfs.furyclient.utils.HFSOnGetImageListener;
import com.hfs.furyclient.utils.Utilities;
import com.hfs.furyclient.views.HFSControlView;
import com.hfs.furyclient.views.HFSDrawingView;
import com.hfs.furyclient.views.HFSInstructionHintView;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.function.Predicate;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

import static android.Manifest.permission.CAMERA;

/**
 * Created by hfs on 30.11.2017.
 */

public class HFSInstructionActivity extends AppCompatActivity
        implements HFSInstructionHintView.HFSHintViewClickListener,
        HFSControlView.HFSControlViewListener,
        ActivityCompat.OnRequestPermissionsResultCallback,
        HFSOnGetImageListener.TrackerFocusListener {
    public static final String EXTRA_HFS_INSTRUCTION = "com.hfs.furyclient.EXTRA_HFS_INSTRUCTION";
    public static final String Server_URL = "http://82.202.212.217/detect_lamp";
    private static final int REQUEST_CODE_CAMERA_PERMISSIONS = 1;
    private static final int MINIMUM_CAMERA_PREVIEW_SIZE = 300;
    private final HFSOnGetImageListener mOnGetPreviewListener = new HFSOnGetImageListener();
    private final CameraCaptureSession.CaptureCallback captureCallback =
            new CameraCaptureSession.CaptureCallback() {
                @Override
                public void onCaptureProgressed(
                        @NonNull final CameraCaptureSession session,
                        @NonNull final CaptureRequest request,
                        @NonNull final CaptureResult partialResult) {
                }

                @Override
                public void onCaptureCompleted(
                        @NonNull final CameraCaptureSession session,
                        @NonNull final CaptureRequest request,
                        @NonNull final TotalCaptureResult result) {
                }
            };
    private final Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private final Point mScreenSize = new Point();
    private HFSInstruction mInstruction;
    private HFSInstructionHintView mHintView;
    private HFSControlView mControlView;
    private boolean mInstructionCompleted = false;
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    private HandlerThread mInferenceThread;
    private Handler mInferenceHandler;
    private ImageReader mCameraPreviewReader;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private CaptureRequest mPreviewRequest;
    private Size mCameraPreviewSize;
    private TextureView mTextureView;
    private String mCameraId;
    private int mCameraFacing;
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCaptureSession;
    private final CameraDevice.StateCallback stateCallback =
            new CameraDevice.StateCallback() {
                @Override
                public void onOpened(@NonNull final CameraDevice cd) {
                    // This method is called when the camera is opened.  We start camera preview here.
                    mCameraOpenCloseLock.release();
                    mCameraDevice = cd;
                    createCameraPreviewSession();
                }

                @Override
                public void onDisconnected(@NonNull final CameraDevice cd) {
                    mCameraOpenCloseLock.release();
                    cd.close();
                    mCameraDevice = null;
                }

                @Override
                public void onError(@NonNull final CameraDevice cd, final int error) {
                    mCameraOpenCloseLock.release();
                    cd.close();
                    mCameraDevice = null;
                    finish();
                }
            };
    private final TextureView.SurfaceTextureListener mSurfaceTextureListener =
            new TextureView.SurfaceTextureListener() {
                @Override
                public void onSurfaceTextureAvailable(
                        final SurfaceTexture texture, final int width, final int height) {
                    openCamera(width, height);
                }

                @Override
                public void onSurfaceTextureSizeChanged(
                        final SurfaceTexture texture, final int width, final int height) {
                    configureTransform(width, height);
                }

                @Override
                public boolean onSurfaceTextureDestroyed(final SurfaceTexture texture) {
                    return true;
                }

                @Override
                public void onSurfaceTextureUpdated(final SurfaceTexture texture) {
                }
            };
    private HFSObjTracker mObjTracker;
    private HFSDrawingView mDrawingView;

    private static Size chooseOptimalSize(Context context, Size[] choices) {
        // Collect the supported resolutions that are at least as big as the preview Surface
        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        Point point = new Point();
        display.getRealSize(point);
        int screen_width = point.x;
        int screen_height = point.y;
        float screenRatio = (float) screen_width / screen_height;
        final List<Size> bigEnough = new ArrayList<>();
        for (final Size option : choices) {
            int height = option.getHeight();
            int width = option.getWidth();
            float cameraOutputRatio = (float) height / width;
            if (screenRatio == cameraOutputRatio) {
                if (height >= MINIMUM_CAMERA_PREVIEW_SIZE && width >= MINIMUM_CAMERA_PREVIEW_SIZE) {
                    bigEnough.add(option);
                }
            } else {
            }
        }

        // Pick the smallest of those, assuming we found any
        if (bigEnough.size() > 0) {
            final Size chosenSize = Collections.min(bigEnough, new CompareSizesByArea());
            HFSDrawingView.scale = (float) screen_height / (float) chosenSize.getWidth() / HFSOnGetImageListener.scale;
            return chosenSize;
        } else {
            return choices[0];
        }
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_instruction);
        mInstruction = getIntent().getParcelableExtra(EXTRA_HFS_INSTRUCTION);
        Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);

        mCameraFacing = CameraCharacteristics.LENS_FACING_BACK;
        mTextureView = (TextureView) findViewById(R.id.texture);
        mControlView = findViewById(R.id.instruction_control_view);
        mControlView.setListener(this);
        mDrawingView = findViewById(R.id.instruction_drawing_view);
    }

    @Override
    protected void onResume() {
        super.onResume();

        mTextureView.setAlpha(1f);
        startBackgroundThread();
        checkPermissions();
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopListenToCamera();
        HFSObjectTracker.release();
        stopBackgroundThread();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelable(EXTRA_HFS_INSTRUCTION, mInstruction);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        mInstruction = savedInstanceState.getParcelable(EXTRA_HFS_INSTRUCTION);
        initInstruction();
    }

    /**
     * Checks camera permissions.
     *
     * @return
     */
    private boolean checkPermissions() {
        if (ContextCompat.checkSelfPermission(this, CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{CAMERA},
                    REQUEST_CODE_CAMERA_PERMISSIONS);
            return false;
        }
        initInstruction();
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_CODE_CAMERA_PERMISSIONS) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                initInstruction();
            } else {
                finish();
            }
        }
    }

    /**
     * Initializes the hint view.
     */
    private void initInstruction() {
        if (mInstruction != null) {
            if (mHintView == null) {
                mHintView = findViewById(R.id.instruction_hint_view);
            }
            mHintView.setListener(this);
            loadStep(mInstruction.getCurrentStepId());
            // TODO: move next two lines to smthng like 'on server response'
            startListenToCamera();
            mOnGetPreviewListener.initialize(this, mInferenceHandler, mDrawingView, null);
        } else {
            Toast.makeText(this, "Instruction is null!", Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    /**
     * Shows the hint view.
     *
     * @param show
     */
    private void showHint(boolean show) {
        if (mHintView != null) {
            mHintView.setVisibility(show ? View.VISIBLE : View.GONE);
        }
        if (mControlView != null) {
            mControlView.setVisibility(show ? View.GONE : View.VISIBLE);
        }
    }

    /**
     * Updates the hint.
     */
    private void updateHint() {
        checkInstructionCompleted();
        if (mHintView != null) {
            mHintView.updateHintText(
                    mInstructionCompleted ?
                            String.format(getResources().getString(
                                    R.string.isntruction_completed_text), mInstruction.name) :
                            mInstruction.getCurrentStep().hint);
        }
        showHint(true);
    }

    /**
     * Checks the instruction is completed.
     */
    private void checkInstructionCompleted() {
        if (mInstructionCompleted) {
            if (mControlView != null) {
                mControlView.setVisibility(View.GONE);
            }
        }
    }

    /**
     * Loads the step with identifier = id.
     *
     * @param id
     */
    private void loadStep(int id) {
        if (id >= mInstruction.getStepsCount()) {
            mInstructionCompleted = true;
        } else {
            mInstruction.setCurrentStepId(id);
        }
        if (mControlView != null) {
            mControlView.enablePrevious(id > 0);
        }
        updateHint();
        mOnGetPreviewListener.setOperational(true);
    }

    /**
     * Loads the ar hint for current step
     *
     * @param id Step Id
     */
    private void loadArHint(int id) {

    }

    private void startListenToCamera() {
        // When the screen is turned off and turned back on, the SurfaceTexture is already
        // available, and "onSurfaceTextureAvailable" will not be called. In that case, we can open
        // a camera and start preview from here (otherwise, we wait until the surface is ready in
        // the SurfaceTextureListener).
        if (mTextureView.isAvailable()) {
            openCamera(mTextureView.getWidth(), mTextureView.getHeight());
        } else {
            mTextureView.setSurfaceTextureListener(mSurfaceTextureListener);
        }
        mOnGetPreviewListener.setOperational(true);
    }

    private void stopListenToCamera() {
        mOnGetPreviewListener.setOperational(false);
        closeCamera();
    }

    /**
     * Creates the camera preview session.
     */
    private void createCameraPreviewSession() {
        try {
            final SurfaceTexture texture = mTextureView.getSurfaceTexture();
            assert texture != null;

            // We configure the size of default buffer to be the size of camera preview we want.
            texture.setDefaultBufferSize(mCameraPreviewSize.getWidth(), mCameraPreviewSize.getHeight());

            // This is the output Surface we need to start preview.
            final Surface surface = new Surface(texture);

            // We set up a CaptureRequest.Builder with the output Surface.
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(surface);

            // Create the reader for the preview frames.
            mCameraPreviewReader =
                    ImageReader.newInstance(
                            mCameraPreviewSize.getWidth(), mCameraPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);

            mCameraPreviewReader.setOnImageAvailableListener(mOnGetPreviewListener, mBackgroundHandler);
            mPreviewRequestBuilder.addTarget(mCameraPreviewReader.getSurface());

            // Here, we create a CameraCaptureSession for camera preview.
            mCameraDevice.createCaptureSession(
                    Arrays.asList(surface, mCameraPreviewReader.getSurface()), new CameraCaptureSession.StateCallback() {

                        @Override
                        public void onConfigured(@NonNull final CameraCaptureSession cameraCaptureSession) {
                            // The camera is already closed
                            if (null == mCameraDevice) {
                                return;
                            }

                            // When the session is ready, we start displaying the preview.
                            mCaptureSession = cameraCaptureSession;
                            try {
                                // Auto focus should be continuous for camera preview.
                                mPreviewRequestBuilder.set(
                                        CaptureRequest.CONTROL_AF_MODE,
                                        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                                // Flash is automatically enabled when necessary.
                                mPreviewRequestBuilder.set(
                                        CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);

                                // Finally, we start displaying the camera preview.
                                mPreviewRequest = mPreviewRequestBuilder.build();
                                mCaptureSession.setRepeatingRequest(
                                        mPreviewRequest, captureCallback, mBackgroundHandler);
                            } catch (final CameraAccessException ignored) {
                            }
                        }

                        @Override
                        public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {
                            Toast.makeText(getApplicationContext(), "Failed", Toast.LENGTH_SHORT).show();
                        }
                    },
                    null);
        } catch (final CameraAccessException ignored) {
        }
    }

    /**
     * Sets up member variables related to camera.
     */
    private void setUpCameraOutputs() {
        final CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try {
            // Check the facing types of camera devices
            @SuppressWarnings("ConstantConditions") String[] cameraList = manager.getCameraIdList();
            for (final String cameraId : cameraList) {
                final CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
                final Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && facing == mCameraFacing) {
                    final StreamConfigurationMap map =
                            characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

                    if (map == null) {
                        continue;
                    }

                    // Danger, W.R.! Attempting to use too large a preview size could  exceed the camera
                    // bus' bandwidth limitation, resulting in gorgeous previews but the storage of
                    // garbage capture data.
                    mCameraPreviewSize = chooseOptimalSize(this, map.getOutputSizes(SurfaceTexture.class));
                    HFSOnGetImageListener.scale = (float) 0.5f;
                    int width = (int) (mCameraPreviewSize.getWidth() * HFSOnGetImageListener.scale);
                    int height = (int) (mCameraPreviewSize.getHeight() * HFSOnGetImageListener.scale);
                    mCameraId = cameraId;
                    return;
                }
            }

        } catch (final CameraAccessException | NullPointerException ignored) {
        }
    }

    /**
     * Opens the camera.
     */
    private void openCamera(final int width, final int height) {
        setUpCameraOutputs();
        configureTransform(width, height);
        final CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try {
            if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                    != PackageManager.PERMISSION_GRANTED) {
            }
            //noinspection ConstantConditions
            manager.openCamera(mCameraId, stateCallback, mBackgroundHandler);
        } catch (final CameraAccessException e) {
        } catch (final InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    /**
     * Closes the camera.
     */
    private void closeCamera() {
        try {
            mCameraOpenCloseLock.acquire();
            if (null != mCaptureSession) {
                mCaptureSession.close();
                mCaptureSession = null;
            }
            if (null != mCameraDevice) {
                mCameraDevice.close();
                mCameraDevice = null;
            }
            if (null != mCameraPreviewReader) {
                mCameraPreviewReader.close();
                mCameraPreviewReader = null;
            }
        } catch (final InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    /**
     * Configures the transform.
     *
     * @param viewWidth
     * @param viewHeight
     */
    private void configureTransform(final int viewWidth, final int viewHeight) {
        if (null == mTextureView || null == mCameraPreviewSize) {
            return;
        }
        final int rotation = getWindowManager().getDefaultDisplay().getRotation();
        final Matrix matrix = new Matrix();
        final RectF viewRect = new RectF(0, 0, viewWidth, viewHeight);
        final RectF bufferRect = new RectF(0, 0, mCameraPreviewSize.getHeight(), mCameraPreviewSize.getWidth());
        final float centerX = viewRect.centerX();
        final float centerY = viewRect.centerY();
        if (Surface.ROTATION_90 == rotation || Surface.ROTATION_270 == rotation) {
            bufferRect.offset(centerX - bufferRect.centerX(), centerY - bufferRect.centerY());
            matrix.setRectToRect(viewRect, bufferRect, Matrix.ScaleToFit.FILL);
            final float scale =
                    Math.max(
                            (float) viewHeight / mCameraPreviewSize.getHeight(),
                            (float) viewWidth / mCameraPreviewSize.getWidth());
            matrix.postScale(scale, scale, centerX, centerY);
            matrix.postRotate(90 * (rotation - 2), centerX, centerY);
        } else if (Surface.ROTATION_180 == rotation) {
            matrix.postRotate(180, centerX, centerY);
        }
    }

    /**
     * Starts a background thread.
     */
    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("ImageListener");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());

        mInferenceThread = new HandlerThread("InferenceThread");
        mInferenceThread.start();
        mInferenceHandler = new Handler(mInferenceThread.getLooper());
    }

    /**
     * Stops the background thread.
     */
    private void stopBackgroundThread() {
        mBackgroundThread.quitSafely();
        mInferenceThread.quitSafely();
        try {
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;

            mInferenceThread.join();
            mInferenceThread = null;
            mInferenceThread = null;
        } catch (final InterruptedException ignored) {
        }
    }

    // region HFSInstructionHintViewListener
    @Override
    public void OnGotItClick() {
        if (mInstructionCompleted) {
            finish();
        } else {
            showHint(false);
        }
    }

    // region HFSControlViewListener
    @Override
    public void OnCloseClick() {
        finish();
    }
    // endregion

    @Override
    public void OnNextStepClick() {
        mOnGetPreviewListener.initialize(this, mInferenceHandler, mDrawingView, null);
        loadStep(mInstruction.getCurrentStepId() + 1);
    }

    @Override
    public void OnPreviousStepClick() {

        mOnGetPreviewListener.initialize(this, mInferenceHandler, mDrawingView, null);
        loadStep(mInstruction.getCurrentStepId() - 1);
    }

    @Override
    public void OnArHintClick() {
        loadArHint(mInstruction.getCurrentStepId() - 1);
        detect();
    }

    @Override
    public void onObjectsLosted() {
        detect();
    }

    /**
     * Compare sizes by area class.
     */
    private static class CompareSizesByArea implements Comparator<Size> {
        @Override
        public int compare(final Size lhs, final Size rhs) {
            // We cast here to ensure the multiplications won't overflow
            return Long.signum(
                    (long) lhs.getWidth() * lhs.getHeight() - (long) rhs.getWidth() * rhs.getHeight());
        }
    }

    private void detect() {
        if(mOnGetPreviewListener.mCroppedBitmap != null) {
            try{
                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                mOnGetPreviewListener.mCroppedBitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream);
                byte[] byteArray = stream.toByteArray();
                post(Server_URL, byteArray);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void showAlert() {
        AlertDialog.Builder builder = new AlertDialog.Builder(HFSInstructionActivity.this);
        builder.setTitle(R.string.alert_title)
                .setMessage(R.string.alert_msg)
                .setCancelable(false)
                .setPositiveButton(R.string.button_ok, null);
        AlertDialog alert = builder.create();
        alert.show();
    }

    void post (String postUrl, byte [] postBody) throws IOException {

        MediaType MEDIA_TYPE_JPG = MediaType.parse("image/jpg");
        OkHttpClient.Builder builder = new OkHttpClient.Builder();
        builder.connectTimeout(30, TimeUnit.SECONDS);
        builder.readTimeout(30, TimeUnit.SECONDS);
        builder.writeTimeout(30, TimeUnit.SECONDS);
        OkHttpClient client = builder.build();
        RequestBody body = new MultipartBody.Builder()
                .setType(MultipartBody.FORM).addFormDataPart(
                        "img", "photo", RequestBody.create(MEDIA_TYPE_JPG, postBody))
                .build();

        Request request = new Request.Builder().url(postUrl).post(body).build();
        Call call = client.newCall(request);

        call.enqueue(new Callback() {
            @Override
            public void onFailure(Call call, final IOException e) {
                e.printStackTrace();
                HFSInstructionActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(HFSInstructionActivity.this,
                                e.getMessage().toString(), Toast.LENGTH_SHORT)
                            .show();
                    }
                });
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {

                if(response.isSuccessful()) {
                    System.out.println(response);
                    final String resp = response.body().string();
                    //final String debugResp = "{ \"status_code\": 100, \"detected_objects\": [ { \"class_name\": \"bar\", \"bounding_box\": { \"y\": 10.01953125, \"x\": 10.165283203125, \"w\": 50.0528259277344, \"h\": 50.9102783203125 } }, { \"class_name\": \"base_top\", \"bounding_box\": { \"y\": 200.6915283203125, \"x\": 200.097412109375, \"w\": 50.0380554199219, \"h\": 50.7102355957031 } }, { \"class_name\": \"plafond\", \"bounding_box\": { \"y\": 1148.1287841796875, \"x\": 555.243408203125, \"w\": 260.3699035644531, \"h\": 227.62155151367188 } }, { \"class_name\": \"base_bottom\", \"bounding_box\": { \"y\": 1533.3231201171875, \"x\": 2485.0751953125, \"w\": 390.766357421875, \"h\": 378.9221496582031 } } ] }";
                    // get List objects from JSON
                    HFSInstructionActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            final ArrayList<DetectedObject> objects = Utilities.getObjectsFromJSON(resp);
                            ArrayList<String> detectList = new ArrayList<String>();
                            for(DetectedObject object :objects) {
                                detectList.add(object.getClassName());
                            }
                            ArrayList<String> instructedObj = mInstruction.getCurrentStep().details;
                            if (detectList.containsAll(instructedObj)) {
                                List<Rect> detectedRects = new ArrayList<>();
                                if (objects != null && objects.size() >= 2) {
                                    for (int i = 0; i < 3; i++) {
                                        DetectedObject o = objects.get(i);
                                        double left = o.getX();
                                        double top = o.getY();
                                        double right = o.getX() + o.getW();
                                        double bottom = o.getY() + o.getH();
                                        detectedRects.add(new Rect((int)left ,(int)top, (int)right, (int)bottom));
                                    }
                                }
                                mOnGetPreviewListener.initialize(HFSInstructionActivity.this, mInferenceHandler, mDrawingView, detectedRects);
                                Toast.makeText(HFSInstructionActivity.this, "Detected objects count: " + objects.size(), Toast.LENGTH_SHORT).show();
                            } else {
                                showAlert();
                            }

                        }
                    });

                } else {
                    System.out.println("NotSuccess");
                }
            }
        });
    }

    // endregion
}
