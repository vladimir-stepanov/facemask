package com.huawei.facemask;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
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
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.huawei.dlib.DlibFaceDetector;
import com.huawei.utils.MediaUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import grapevine.face1.MainLib;

public class CameraActivity extends Activity implements View.OnClickListener {

    private static final String TAG = "CameraActivity";
    private static final String ERROR_DIALOG = "error_dialog";
    private static final String KEY_PREVIEW_VISIBLE = "key_preview_visible";
    private static final String KEY_DETECTION_FEATURE = "key_detection_feature";
    private static final String KEY_LANDMARKS_DETECTION = "key_landmarks_detection";
    private static final int MINIMUM_CAMERA_PREVIEW_SIZE = 300;
    private static final int SEEK_BAR_MAX = 20;
    private static final int SOURCE_CAMERA = 1;
    private static final int SOURCE_IMAGES = 2;
    private static final int SOURCE_MOVIE = 3;
    private static final int REQUEST_PERMISSIONS_CODE = 1;
    private static final int OVERLAY_PERMISSION_REQ_CODE = 2;
    private static final String[] PERMISSIONS_REQ = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.CAMERA
    };
    /**
     * A {@link Semaphore} to prevent the app from exiting before closing the camera.
     */
    private final Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private final Point mScreenSize = new Point();
    private final OnGetImageListener mOnGetPreviewListener = new OnGetImageListener();
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

    private TextView mScore;
    private TextView mCameraPreviewFrameRate;
    private TextView mMouthOpen;
    private View mTopPanel;
    private ImageView mSwitchCameraButton;
    private ImageView mMediaPlayPauseButton;

    private View mBottomPanel;
    private TextView mImageSizeCaption;
    /**
     * {@link android.view.TextureView.SurfaceTextureListener} handles several lifecycle events on a
     * {@link TextureView}.
     */
    private final TextureView.SurfaceTextureListener surfaceTextureListener =
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
                    if (mSourceForRecognition == SOURCE_MOVIE) {
                        Bitmap bmp = mTextureView.getBitmap();
                        mFloatingWindow.setRGBBitmap(bmp);
                    }
                }
            };
    private TextView mImageBrightnessCaption;
    private TextView mImageContrastCaption;
    private ImageView mVisibilitySwitcher;
    private FloatingPreviewWindow mFloatingWindow;
    private SharedPreferences mPreferences;
    private DlibFaceDetector mDlibFaceDetector;
    private RadioButton mDlibRadioButton;
    private RadioButton mGmsRadioButton;
    private ImageView mLandmarksDetection;
    private boolean mLandmarksDetectionEnabled;
    /**
     * {@link android.hardware.camera2.CameraDevice.StateCallback}
     * is called when {@link CameraDevice} changes its state.
     */
    private final CameraDevice.StateCallback stateCallback =
            new CameraDevice.StateCallback() {
                @Override
                public void onOpened(@NonNull final CameraDevice cd) {
                    // This method is called when the camera is opened.  We start camera preview here.
                    mCameraOpenCloseLock.release();
                    mCameraDevice = cd;
                    createCameraPreviewSession();
                    if (mFloatingWindow == null) {
                        createFloatingView();
                    }
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
    private SeekBar mBrightnessBar;
    private SeekBar mContrastBar;
    private SeekBar mPreviewSizeBar;
    private int mSourceForRecognition;
    private ImageView mSourceCameraButton;

    //    private FaceView mFaceView;
    private ImageView mSourceImagesButton;
    private ImageView mSourceVideoButton;

    /**
     * Given {@code choices} of {@code Size}s supported by a camera, chooses the smallest one whose
     * width and height are at least as large as the respective requested values, and whose aspect
     * ratio matches with the specified value.
     *
     * @param choices The list of sizes that the camera supports for the intended output class
     * @return The optimal {@code Size}, or an arbitrary one if none were big enough
     */
    @SuppressLint("LongLogTag")
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
                Log.i(TAG, "Not adding size: " + option.getWidth() + "x" + option.getHeight());
            }
        }

        // Pick the smallest of those, assuming we found any
        if (bigEnough.size() > 0) {
            final Size chosenSize = Collections.min(bigEnough, new CompareSizesByArea());
            Log.i(TAG, "Chosen size: " + chosenSize.getWidth() + "x" + chosenSize.getHeight());
            return chosenSize;
        } else {
            Log.e(TAG, "Couldn't find any suitable preview size");
            return choices[0];
        }
    }

    @Override
    protected void onCreate(final Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_camera);
        mTextureView = (TextureView) findViewById(R.id.texture);
        mTextureView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                onClickCameraView();
            }
        });
        Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);

        mScore = (TextView) findViewById(R.id.recognition_time);
        mScore.setText(R.string.initializing_engine);
        mCameraPreviewFrameRate = (TextView) findViewById(R.id.camera_preview_frame_rate);
        mCameraPreviewFrameRate.setText(getResources().getString(R.string.camera_preview_frame_rate, 30.0f));
        mMouthOpen = (TextView) findViewById(R.id.mouth_open);
        mMouthOpen.setText(getResources().getString(R.string.mouth_open, 0f));
        mTopPanel = findViewById(R.id.topPanel);

        mCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;
        mSourceForRecognition = SOURCE_CAMERA;
        mSourceCameraButton = (ImageView) findViewById(R.id.source_camera);
        mSourceCameraButton.setOnClickListener(this);
        mSourceCameraButton.setImageResource(R.drawable.ic_camera_selected);
        mSourceImagesButton = (ImageView) findViewById(R.id.source_images);
        mSourceImagesButton.setOnClickListener(this);
        mSourceVideoButton = (ImageView) findViewById(R.id.source_video);
        mSourceVideoButton.setOnClickListener(this);

        mSwitchCameraButton = (ImageView) findViewById(R.id.switch_camera_button);
        mSwitchCameraButton.setOnClickListener(this);
        mMediaPlayPauseButton = (ImageView) findViewById(R.id.media_play_pause_button);
        mMediaPlayPauseButton.setOnClickListener(this);

        mBottomPanel = findViewById(R.id.bottom_panel);
        mImageSizeCaption = (TextView) findViewById(R.id.image_size_caption);

        mPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mOnGetPreviewListener.setContrast(1.0f);
        mOnGetPreviewListener.setBrightness(0f);
        ImageHandler.setContrast(1.0f);
        ImageHandler.setBrightness(0f);
        MovieHandler.setContrast(1.0f);
        MovieHandler.setBrightness(0f);
        int feature = mPreferences.getInt(KEY_DETECTION_FEATURE, OnGetImageListener.DLIB_FACE_RECOGNITION);
        ImageHandler.setFaceRecognition(feature);
        MovieHandler.setFaceRecognition(feature);
        mOnGetPreviewListener.setFaceRecognition(feature);
        mImageBrightnessCaption = (TextView) findViewById(R.id.image_brightness_caption);
        mImageBrightnessCaption.setText(getString(R.string.brightness_caption, 0f));
        mImageContrastCaption = (TextView) findViewById(R.id.image_contrast_caption);
        mImageContrastCaption.setText(getString(R.string.contrast_caption, 1f));
        mPreviewSizeBar = (SeekBar) findViewById(R.id.preview_size_seek_bar);
        mPreviewSizeBar.setMax(SEEK_BAR_MAX);
        mPreviewSizeBar.setProgress((int) (OnGetImageListener.SCALE * SEEK_BAR_MAX));
        mPreviewSizeBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // Set size of image for recognition
                if (progress == 0) {
                    progress = 1;
                }
                OnGetImageListener.SCALE = (float) progress / SEEK_BAR_MAX;
                int width = (int) (mCameraPreviewSize.getWidth() * OnGetImageListener.SCALE);
                int height = (int) (mCameraPreviewSize.getHeight() * OnGetImageListener.SCALE);
                mImageSizeCaption.setText(getString(R.string.image_size, width, height));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        mBrightnessBar = (SeekBar) findViewById(R.id.brightness_seek_bar);
        mBrightnessBar.setMax(510);
        mBrightnessBar.setProgress(255);
        mBrightnessBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float brightness = progress - 255;
                mOnGetPreviewListener.setBrightness(brightness);
                ImageHandler.setBrightness(brightness);
                MovieHandler.setBrightness(brightness);
                mImageBrightnessCaption.setText(getString(R.string.brightness_caption, brightness));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        ImageView brightnessButton = (ImageView) findViewById(R.id.brightness_default);
        brightnessButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mBrightnessBar.setProgress(255);
            }
        });
        mContrastBar = (SeekBar) findViewById(R.id.contrast_seek_bar);
        mContrastBar.setMax(200);
        mContrastBar.setProgress(100);
        mContrastBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float contrast;
                if (progress == 100) {
                    contrast = 1.0f;
                } else if (progress > 100) {
                    contrast = 1.0f + (progress - 100.0f) / 11.111111111111f;

                } else {
                    contrast = (float) progress / 100.0f;
                }
                mOnGetPreviewListener.setContrast(contrast);
                ImageHandler.setContrast(contrast);
                MovieHandler.setContrast(contrast);
                mImageContrastCaption.setText(getString(R.string.contrast_caption, contrast));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        ImageView contrastButton = (ImageView) findViewById(R.id.contrast_default);
        contrastButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mContrastBar.setProgress(100);
            }
        });
        mVisibilitySwitcher = (ImageView) findViewById(R.id.visibility_switcher);
        mVisibilitySwitcher.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean visible = !mPreferences.getBoolean(KEY_PREVIEW_VISIBLE, true);
                if (visible) {
                    mVisibilitySwitcher.setImageResource(R.drawable.preview_ic_visibility);
                    mFloatingWindow.show();
                } else {
                    mVisibilitySwitcher.setImageResource(R.drawable.preview_ic_visibility_off);
                    mFloatingWindow.hide();
                }
                mPreferences.edit().putBoolean(KEY_PREVIEW_VISIBLE, visible).apply();
            }
        });
        mLandmarksDetectionEnabled = mPreferences.getBoolean(KEY_LANDMARKS_DETECTION, true);
        mLandmarksDetection = (ImageView) findViewById(R.id.landmarks_detection_switcher);
        mLandmarksDetection.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mLandmarksDetectionEnabled = !mPreferences.getBoolean(KEY_LANDMARKS_DETECTION, true);
                if (mLandmarksDetectionEnabled) {
                    mLandmarksDetection.setImageResource(R.drawable.landmarks_ic_on);
                } else {
                    mLandmarksDetection.setImageResource(R.drawable.landmarks_ic_off);
                }
                mPreferences.edit().putBoolean(KEY_LANDMARKS_DETECTION, mLandmarksDetectionEnabled).apply();
                GmsVision.setLandmarksDetection(CameraActivity.this, mLandmarksDetectionEnabled);
                Dlib.setLandmarksDetection(mLandmarksDetectionEnabled);
            }
        });
        if (mLandmarksDetectionEnabled) {
            mLandmarksDetection.setImageResource(R.drawable.landmarks_ic_on);
        } else {
            mLandmarksDetection.setImageResource(R.drawable.landmarks_ic_off);
        }

        mDlibRadioButton = (RadioButton) findViewById(R.id.dlib_radio_button);
        mDlibRadioButton.setChecked(true);
        mGmsRadioButton = (RadioButton) findViewById(R.id.gms_radio_button);
        mGmsRadioButton.setChecked(false);
        mDlibRadioButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDlibRadioButton.setChecked(true);
                mGmsRadioButton.setChecked(false);
                ImageHandler.setFaceRecognition(OnGetImageListener.DLIB_FACE_RECOGNITION);
                MovieHandler.setFaceRecognition(OnGetImageListener.DLIB_FACE_RECOGNITION);
                mOnGetPreviewListener.setFaceRecognition(OnGetImageListener.DLIB_FACE_RECOGNITION);
                mPreferences.edit().putInt(KEY_DETECTION_FEATURE, OnGetImageListener.DLIB_FACE_RECOGNITION).apply();
            }
        });
        mGmsRadioButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGmsRadioButton.setChecked(true);
                mDlibRadioButton.setChecked(false);
                ImageHandler.setFaceRecognition(OnGetImageListener.GMS_FACE_RECOGNITION);
                MovieHandler.setFaceRecognition(OnGetImageListener.GMS_FACE_RECOGNITION);
                mOnGetPreviewListener.setFaceRecognition(OnGetImageListener.GMS_FACE_RECOGNITION);
                mPreferences.edit().putInt(KEY_DETECTION_FEATURE, OnGetImageListener.GMS_FACE_RECOGNITION).apply();
            }
        });

        mDlibRadioButton.setChecked(feature == OnGetImageListener.DLIB_FACE_RECOGNITION);
        mGmsRadioButton.setChecked(feature == OnGetImageListener.GMS_FACE_RECOGNITION);
    }

    private void RequestDrawOverlays() {
        if (!Settings.canDrawOverlays(this.getApplicationContext())) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, OVERLAY_PERMISSION_REQ_CODE);
        }
    }

    private void verifyPermissions(Activity activity) {
        // Check if we have write permission
        int write_permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        int read_permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        int camera_permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.CAMERA);

        if (write_permission != PackageManager.PERMISSION_GRANTED ||
                read_permission != PackageManager.PERMISSION_GRANTED ||
                camera_permission != PackageManager.PERMISSION_GRANTED) {
            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    PERMISSIONS_REQ,
                    REQUEST_PERMISSIONS_CODE
            );
        } else {
            mDlibFaceDetector = DlibFaceDetector.getInstance(this);
            mDlibFaceDetector.asyncInit();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        if (requestCode == REQUEST_PERMISSIONS_CODE) {
            for (int result : grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    finish();
                    return;
                }
            }
            mDlibFaceDetector = DlibFaceDetector.getInstance(this);
            mDlibFaceDetector.asyncInit();
        }
    }

    private void onClickCameraView() {
        final boolean bottom = mBottomPanel.getVisibility() == View.VISIBLE;
        final boolean top = mTopPanel.getVisibility() == View.VISIBLE;
        if (top && bottom) {
            mTopPanel.setVisibility(View.GONE);
            mBottomPanel.setVisibility(View.GONE);
        } else if (top) {
            mBottomPanel.setVisibility(View.VISIBLE);
        } else {
            mTopPanel.setVisibility(View.VISIBLE);
        }
    }

    private void startListenToMovie() {
        AssetManager assetManager = getAssets();
        MainLib.onCreate();
        MainLib.setAssetManager(assetManager);
        if (mFloatingWindow != null) {
            mFloatingWindow.show();
        }
    }

    private void stopListenToMovie() {
        MainLib.onDestroy();
        MovieHandler.stop();
    }

    private void startListenToCamera() {
        AssetManager assetManager = getAssets();
        MainLib.onCreate();
        MainLib.setAssetManager(assetManager);

        // When the screen is turned off and turned back on, the SurfaceTexture is already
        // available, and "onSurfaceTextureAvailable" will not be called. In that case, we can open
        // a camera and start preview from here (otherwise, we wait until the surface is ready in
        // the SurfaceTextureListener).
        if (mTextureView.isAvailable()) {
            openCamera(mTextureView.getWidth(), mTextureView.getHeight());
        } else {
            mTextureView.setSurfaceTextureListener(surfaceTextureListener);
        }
        mOnGetPreviewListener.setOperational(true);
        if (mFloatingWindow != null) {
            mFloatingWindow.show();
        }
    }

    private void stopListenToCamera() {
        mOnGetPreviewListener.setOperational(false);
        closeCamera();
        MainLib.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mScore.setText(R.string.initializing_engine);
        verifyPermissions(this);
        RequestDrawOverlays();

/*
        mFaceView = new FaceView(getApplication(), true, 24, 0); // Translucent, 24-bit depth, no stencil
        FrameLayout layout = (FrameLayout) findViewById(R.id.frame_layout);
        layout.addView(mFaceView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        mFaceView.setZOrderOnTop(true);
        mFaceView.onResume();
        mFaceView.bringToFront();
*/

        startBackgroundThread();
        startListenToCamera();
    }

    @Override
    protected void onPause() {
        super.onPause();
//        mFaceView.onPause();
        stopListenToCamera();
        Dlib.release();
        GmsVision.release();
        if (mFloatingWindow != null) {
            mFloatingWindow.release();
            mFloatingWindow = null;
        }
        stopBackgroundThread();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == OVERLAY_PERMISSION_REQ_CODE) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                if (!Settings.canDrawOverlays(this.getApplicationContext())) {
                    Toast.makeText(CameraActivity.this, "CameraActivity\"," +
                            "\"SYSTEM_ALERT_WINDOW, permission not granted...", Toast.LENGTH_SHORT).show();
                } else {
                    Intent intent = getIntent();
                    finish();
                    startActivity(intent);
                }
            }
        }
    }

    /**
     * Sets up member variables related to camera.
     */
    @SuppressLint("LongLogTag")
    private void setUpCameraOutputs() {
        final CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try {
            // Check the facing types of camera devices
            String[] cameraList = manager.getCameraIdList();
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
                    int width = (int) (mCameraPreviewSize.getWidth() * OnGetImageListener.SCALE);
                    int height = (int) (mCameraPreviewSize.getHeight() * OnGetImageListener.SCALE);
                    mImageSizeCaption.setText(getString(R.string.image_size, width, height));
                    mCameraId = cameraId;
                    return;
                }
            }

        } catch (final CameraAccessException e) {
            Log.e(TAG, "Exception!", e);
        } catch (final NullPointerException e) {
            // Currently an NPE is thrown when the Camera2API is used but not supported on the
            // device this code runs.
            ErrorDialog.newInstance(getString(R.string.camera_error))
                    .show(getFragmentManager(), ERROR_DIALOG);
        }
    }

    /**
     * Opens the camera specified by {@link #mCameraId}.
     */
    @SuppressLint("LongLogTag")
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
                Log.w(TAG, "checkSelfPermission CAMERA");
            }
            manager.openCamera(mCameraId, stateCallback, mBackgroundHandler);
            Log.d(TAG, "open Camera");
        } catch (final CameraAccessException e) {
            Log.e(TAG, "Exception!", e);
        } catch (final InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    /**
     * Closes the current {@link CameraDevice}.
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
     * Starts a background thread and its {@link Handler}.
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
     * Stops the background thread and its {@link Handler}.
     */
    @SuppressLint("LongLogTag")
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
        } catch (final InterruptedException e) {
            Log.e(TAG, "error", e);
        }
    }

    /**
     * Creates a new {@link CameraCaptureSession} for camera preview.
     */
    @SuppressLint("LongLogTag")
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

            Log.i(TAG, "Opening camera preview: " + mCameraPreviewSize.getWidth() + "x" + mCameraPreviewSize.getHeight());

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
                            } catch (final CameraAccessException e) {
                                Log.e(TAG, "Exception!", e);
                            }
                        }

                        @Override
                        public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {
                            Toast.makeText(getApplicationContext(), "Failed", Toast.LENGTH_SHORT).show();
                        }
                    },
                    null);
        } catch (final CameraAccessException e) {
            Log.e(TAG, "Exception!", e);
        }
    }

    private void createFloatingView() {
        mFloatingWindow = new FloatingPreviewWindow(this);

        mOnGetPreviewListener.initialize(this, mFloatingWindow, mScore,
                mCameraPreviewFrameRate, mMouthOpen, mLandmarksDetectionEnabled, mInferenceHandler);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                boolean visible = mPreferences.getBoolean(KEY_PREVIEW_VISIBLE, true);
                if (visible) {
                    mVisibilitySwitcher.setImageResource(R.drawable.preview_ic_visibility);
                    if (mFloatingWindow != null) {
                        mFloatingWindow.show();
                    }
                } else {
                    mVisibilitySwitcher.setImageResource(R.drawable.preview_ic_visibility_off);
                    if (mFloatingWindow != null) {
                        mFloatingWindow.hide();
                    }
                }
            }
        });
    }

    /**
     * Configures the necessary {@link android.graphics.Matrix} transformation to `mTextureView`.
     * This method should be called after the camera preview size is determined in
     * setUpCameraOutputs and also the size of `mTextureView` is fixed.
     *
     * @param viewWidth  The width of `mTextureView`
     * @param viewHeight The height of `mTextureView`
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
        mTextureView.setTransform(matrix);
    }

    private boolean setSourceForRecognition(int source) {
        boolean result = false;
        boolean sourceChanged = mSourceForRecognition != source;
        if (sourceChanged) {
            if (source == SOURCE_IMAGES) {
                if (ImageHandler.init(this, mFloatingWindow)) {
                    if (mSourceForRecognition == SOURCE_CAMERA) {
                        stopListenToCamera();
                    }
                    if (mSourceForRecognition == SOURCE_MOVIE) {
                        stopListenToMovie();
                    }
                    ImageHandler.init(this, mFloatingWindow);
                    result = true;
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_play);
                    mImageSizeCaption.setText(getString(R.string.image_size,
                            MediaUtils.HORIZONTAL_SIZE_THUMBNAIL, MediaUtils.VERTICAL_SIZE_THUMBNAIL));
                }
            } else if (source == SOURCE_MOVIE) {
                if (MovieHandler.init(this, mFloatingWindow)) {
                    ImageHandler.stop();
                    if (mSourceForRecognition == SOURCE_CAMERA) {
                        stopListenToCamera();
                    }
                    MovieHandler.init(this, mFloatingWindow);
                    result = true;
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_play);
                    mImageSizeCaption.setText(getString(R.string.image_size,
                            MediaUtils.HORIZONTAL_SIZE_THUMBNAIL, MediaUtils.VERTICAL_SIZE_THUMBNAIL));
                    startListenToMovie();
                }
            } else if (source == SOURCE_CAMERA) {
                if (mSourceForRecognition == SOURCE_MOVIE) {
                    stopListenToMovie();
                }
                ImageHandler.stop();
                startListenToCamera();
                result = true;
            }
        }
        if (result) {
            mSourceForRecognition = source;
        }

        mTextureView.setAlpha(mSourceForRecognition == SOURCE_IMAGES ? 0f : 1f);
        mPreviewSizeBar.setVisibility(mSourceForRecognition == SOURCE_CAMERA ? View.VISIBLE : View.INVISIBLE);
        mSwitchCameraButton.setVisibility(mSourceForRecognition == SOURCE_CAMERA ? View.VISIBLE : View.GONE);
        mMediaPlayPauseButton.setVisibility(mSourceForRecognition == SOURCE_CAMERA ? View.GONE : View.VISIBLE);
        return result;
    }

    @Override
    public void onClick(View view) {
        if (view == mSourceCameraButton) {
            if (setSourceForRecognition(SOURCE_CAMERA)) {
                mSourceCameraButton.setImageResource(R.drawable.ic_camera_selected);
                mSourceImagesButton.setImageResource(R.drawable.ic_images);
                mSourceVideoButton.setImageResource(R.drawable.ic_video);
            }
        } else if (view == mSourceImagesButton) {
            if (setSourceForRecognition(SOURCE_IMAGES)) {
                mSourceCameraButton.setImageResource(R.drawable.ic_camera);
                mSourceImagesButton.setImageResource(R.drawable.ic_images_selected);
                mSourceVideoButton.setImageResource(R.drawable.ic_video);
            }
        } else if (view == mSourceVideoButton) {
            if (setSourceForRecognition(SOURCE_MOVIE)) {
                mSourceCameraButton.setImageResource(R.drawable.ic_camera);
                mSourceImagesButton.setImageResource(R.drawable.ic_images);
                mSourceVideoButton.setImageResource(R.drawable.ic_video_selected);
            }
        } else if (view == mSwitchCameraButton) {
            stopListenToCamera();
            if (mCameraFacing == CameraCharacteristics.LENS_FACING_BACK) {
                mCameraFacing = CameraCharacteristics.LENS_FACING_FRONT;
                mSwitchCameraButton.setImageResource(R.drawable.ic_switch_camera_front);
            } else {
                mCameraFacing = CameraCharacteristics.LENS_FACING_BACK;
                mSwitchCameraButton.setImageResource(R.drawable.ic_switch_camera_back);
            }
            mOnGetPreviewListener.setCameraFacing(mCameraFacing);
            startListenToCamera();
        } else if (view == mMediaPlayPauseButton) {
            if (mSourceForRecognition == SOURCE_IMAGES) {
                if (ImageHandler.isRunning()) {
                    ImageHandler.stop();
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_play);
                } else {
                    ImageHandler.start(this, mScore, mMouthOpen, mFloatingWindow);
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_stop);
                }
            }
            if (mSourceForRecognition == SOURCE_MOVIE) {
                if (MovieHandler.isRunning()) {
                    MovieHandler.stop();
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_play);
                } else {
                    MovieHandler.start(this, mTextureView, mScore, mMouthOpen, mFloatingWindow);
                    mMediaPlayPauseButton.setImageResource(R.drawable.ic_media_stop);
                }
            }
        }
    }

    /**
     * Compares two {@code Size}s based on their areas.
     */
    private static class CompareSizesByArea implements Comparator<Size> {
        @Override
        public int compare(final Size lhs, final Size rhs) {
            // We cast here to ensure the multiplications won't overflow
            return Long.signum(
                    (long) lhs.getWidth() * lhs.getHeight() - (long) rhs.getWidth() * rhs.getHeight());
        }
    }

    /**
     * Shows an error message dialog.
     */
    public static class ErrorDialog extends DialogFragment {
        private static final String ARG_MESSAGE = "message";

        public static ErrorDialog newInstance(final String message) {
            final ErrorDialog dialog = new ErrorDialog();
            final Bundle args = new Bundle();
            args.putString(ARG_MESSAGE, message);
            dialog.setArguments(args);
            return dialog;
        }

        @Override
        public Dialog onCreateDialog(final Bundle savedInstanceState) {
            final Activity activity = getActivity();
            return new AlertDialog.Builder(activity)
                    .setMessage(getArguments().getString(ARG_MESSAGE))
                    .setPositiveButton(
                            android.R.string.ok,
                            new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(final DialogInterface dialogInterface, final int i) {
                                    activity.finish();
                                }
                            })
                    .create();
        }
    }
}
