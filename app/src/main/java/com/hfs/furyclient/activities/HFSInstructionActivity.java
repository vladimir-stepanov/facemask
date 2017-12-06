package com.hfs.furyclient.activities;

import android.Manifest;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
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
import com.hfs.furyclient.utils.HFSObjectTracker;
import com.hfs.furyclient.utils.HFSOnGetImageListener;
import com.hfs.furyclient.views.HFSControlView;
import com.hfs.furyclient.views.HFSInstructionHintView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import static android.Manifest.permission.CAMERA;

/**
 * Created by hfs on 30.11.2017.
 */

public class HFSInstructionActivity extends AppCompatActivity
        implements HFSInstructionHintView.HFSHintViewClickListener,
        HFSControlView.HFSControlViewListener,
        ActivityCompat.OnRequestPermissionsResultCallback {
    public static final String EXTRA_HFS_INSTRUCTION = "com.hfs.furyclient.EXTRA_HFS_INSTRUCTION";
    public static final String Server_URL = "http://82.202.212.217/detect";
    public static final int READ_TIMEOUT = 15000;
    public static final int CONNECT_TIMEOUT = 15000;
    public static final String REQUEST_METHOD = "POST";

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
    private HFSObjTracker mObjTracker;

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
        mOnGetPreviewListener.setObjRecognition(HFSOnGetImageListener.MEDIANFLOW_TRACKER);
        mTextureView = (TextureView) findViewById(R.id.texture);
        mControlView = findViewById(R.id.instruction_control_view);
        mControlView.setListener(this);
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
            startListenToCamera();
            mOnGetPreviewListener.initialize(this);
            mObjTracker = HFSObjTracker.getInstance();
            mObjTracker.asyncInit();
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
        HFSOnGetImageListener.clearStatistics();
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
        loadStep(mInstruction.getCurrentStepId() + 1);
    }

    @Override
    public void OnPreviousStepClick() {
        loadStep(mInstruction.getCurrentStepId() - 1);
    }

    @Override
    public void OnArHintClick() {
        loadArHint(mInstruction.getCurrentStepId() - 1);
//        mCameraFragment.onTakePhotoPressed();
        HTTPAsyncTask getTask = new HTTPAsyncTask();
        try {
            getTask.execute(Server_URL).get();
        } catch (Exception ex) {

        }
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

    public class HTTPAsyncTask extends AsyncTask<String, Void, String> {
        @Override
        protected String doInBackground(String... urls) {
            String strUrl = urls[0];
            String line;
            String result;
            try {
                URL url = new URL(strUrl);
                HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                connection.setRequestMethod(REQUEST_METHOD);
                connection.setReadTimeout(READ_TIMEOUT);
                connection.setConnectTimeout(CONNECT_TIMEOUT);

                InputStreamReader isr = new InputStreamReader(connection.getInputStream());
                BufferedReader br = new BufferedReader(isr);
                StringBuilder stringBuilder = new StringBuilder();
                while ((line = br.readLine()) != null) {
                    stringBuilder.append(line);
                }
                br.close();
                isr.close();
                result = stringBuilder.toString();
                connection.connect();
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
            return result;
        }

        // onPostExecute displays the results of the AsyncTask.
        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);
        }
    }

    // endregion


}
