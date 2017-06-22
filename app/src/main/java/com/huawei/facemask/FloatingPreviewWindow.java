package com.huawei.facemask;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.UiThread;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.lang.ref.WeakReference;

class FloatingPreviewWindow {

    private static final int DEFAULT_SCALE = 25;
    private final Context mContext;
    private final Handler mUIHandler;
    private WindowManager.LayoutParams mWindowParam;
    private WindowManager mWindowManager;
    private FloatCamView mRootView;
    private Rect mWindowRect;
    private Point mScreenSize;

    FloatingPreviewWindow(Context context) {
        mContext = context;
        mUIHandler = new Handler(Looper.getMainLooper());
        mScreenSize = new Point();
        Display display = ((WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(mScreenSize);

        // Default window size
        int width = mScreenSize.x * DEFAULT_SCALE / 100;
        int height = mScreenSize.y * DEFAULT_SCALE / 100;
        width = width > 0 && width < mScreenSize.x ? width : mScreenSize.x;
        height = height > 0 && height < mScreenSize.y ? height : mScreenSize.y;
        mWindowRect = new Rect(0, 0, width, height);
    }

    void hide() {
        if (mRootView != null) {
            mRootView.setVisibility(View.GONE);
        }
    }

    void show() {
        if (mRootView != null) {
            mRootView.setVisibility(View.VISIBLE);
        }
    }

    private void init() {
        mUIHandler.postAtFrontOfQueue(new Runnable() {
            @Override
            public void run() {
                if (mWindowManager == null || mRootView == null) {
                    mWindowManager = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
                    mRootView = new FloatCamView(FloatingPreviewWindow.this);
                    mWindowManager.addView(mRootView, initWindowParameter());
                }
            }
        });
    }

    public void release() {
        mUIHandler.postAtFrontOfQueue(new Runnable() {
            @Override
            public void run() {
                if (mWindowManager != null) {
                    mWindowManager.removeViewImmediate(mRootView);
                    mRootView = null;
                }
                mUIHandler.removeCallbacksAndMessages(null);
            }
        });
    }

    private WindowManager.LayoutParams initWindowParameter() {
        mWindowParam = new WindowManager.LayoutParams();

        mWindowParam.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
        mWindowParam.format = 1;
        mWindowParam.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        mWindowParam.flags = mWindowParam.flags | WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH;
        mWindowParam.flags = mWindowParam.flags | WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS;
        mWindowParam.flags = mWindowParam.flags | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN;

        mWindowParam.alpha = 1.0f;

        mWindowParam.gravity = Gravity.TOP | Gravity.START;
        mWindowParam.x = mWindowRect.left;
        mWindowParam.y = mWindowRect.top;
        mWindowParam.width = mWindowRect.width();
        mWindowParam.height = mWindowRect.height();
        return mWindowParam;
    }

    void setRGBBitmap(final Bitmap rgb) {
        checkInit();
        mUIHandler.post(new Runnable() {
            @Override
            public void run() {
                mRootView.setRGBImageView(rgb);
            }
        });
    }

    private void checkInit() {
        if (mRootView == null) {
            init();
        }
    }

    @UiThread
    private final class FloatCamView extends FrameLayout {
        // State objects and values related to gesture tracking.
        private ScaleGestureDetector mScaleGestureDetector;

        private static final int MOVE_THRESHOLD = 10;
        private final WeakReference<FloatingPreviewWindow> mWeakRef;
        private final LayoutInflater mLayoutInflater;
        private final ImageView mColorView;
        private int mLastX;
        private int mLastY;
        private int mFirstX;
        private int mFirstY;
        private boolean mIsMoving = false;
        private boolean mIsScaling = false;
        private long mLastTouchTime = 0;

        public FloatCamView(FloatingPreviewWindow window) {
            super(window.mContext);
            mWeakRef = new WeakReference<>(window);
            mLayoutInflater = (LayoutInflater) window.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mLayoutInflater.inflate(R.layout.cam_window_view, this, true);
            mColorView = (ImageView) findViewById(R.id.preview);
            // Sets up interactions
            mScaleGestureDetector = new ScaleGestureDetector(window.mContext, mScaleGestureListener);
            setSize(mScreenSize.y * DEFAULT_SCALE / 100);
        }

        private void setPos(int left, int top, int width, int height) {
            int right = left + width;
            int bottom = top + height;
            if (right > mScreenSize.x) {
                left = mScreenSize.x - width;
            }
            if (bottom > mScreenSize.y) {
                top = mScreenSize.y - height;
            }
            if (left < 0) {
                left = 0;
            }
            if (top < 0) {
                top = 0;
            }
            mColorView.getLayoutParams().width = width;
            mColorView.getLayoutParams().height = height;
            WindowManager windowMgr = mWeakRef.get().mWindowManager;
            WindowManager.LayoutParams params = mWeakRef.get().mWindowParam;
            if (params != null) {
                params.x = left;
                params.y = top;
                params.width = width;
                params.height = height;
                mWindowRect.left = params.x;
                mWindowRect.top = params.y;
                mWindowRect.right = params.x + width;
                mWindowRect.bottom = params.y + height;
                windowMgr.updateViewLayout(this, params);
            }
        }

        private void setSize(int height) {
            if (height > mScreenSize.y) {
                height = mScreenSize.y;
            }
            int width = height * mScreenSize.x / mScreenSize.y;
            WindowManager.LayoutParams params = mWeakRef.get().mWindowParam;
            if (params != null) {
                int left = params.x + (mWindowRect.width() - width) / 2;
                int top = params.y + (mWindowRect.height() - height) / 2;
                setPos(left, top, width, height);
            }
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            boolean retVal = mScaleGestureDetector.onTouchEvent(event);
            if (!mIsScaling) {
                retVal = onDoubleTapEvent(event);
                if (!retVal) {
                    retVal = onMoveEvent(event);
                }
            }
            return retVal;
        }

        private boolean onDoubleTapEvent(MotionEvent event) {
            boolean retVal = false;
            if (event.getAction() == MotionEvent.ACTION_UP) {
                long timeStamp = System.currentTimeMillis();
                if ((timeStamp - mLastTouchTime) < 500) {
                    mLastTouchTime = 0;
                    if (mWindowRect.height() == mScreenSize.y) {
                        setSize(mScreenSize.y * DEFAULT_SCALE / 100);
                    } else {
                        setPos(0, 0, mScreenSize.x, mScreenSize.y);
                    }
                    retVal = true;
                } else {
                    mLastTouchTime = timeStamp;
                }
            }
            return retVal;
        }

        private boolean onMoveEvent(MotionEvent event) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    mLastX = (int) event.getRawX();
                    mLastY = (int) event.getRawY();
                    mFirstX = mLastX;
                    mFirstY = mLastY;
                    break;
                case MotionEvent.ACTION_MOVE:
                    int deltaX = mLastX - (int) event.getRawX();
                    int deltaY = mLastY - (int) event.getRawY();
                    mLastX = (int) event.getRawX();
                    mLastY = (int) event.getRawY();
                    int totalDeltaX = mFirstX - mLastX;
                    int totalDeltaY = mFirstY - mLastY;
                    if (mIsMoving
                            || Math.abs(totalDeltaX) >= MOVE_THRESHOLD
                            || Math.abs(totalDeltaY) >= MOVE_THRESHOLD) {
                        mIsMoving = true;
                        WindowManager.LayoutParams params = mWeakRef.get().mWindowParam;
                        setPos(params.x - deltaX, params.y - deltaY, params.width, params.height);
                    }
                    break;
                case MotionEvent.ACTION_UP:
                    mIsMoving = false;
                    break;
            }
            return true;
        }

        /**
         * The scale listener, used for handling multi-finger scale gestures.
         */
        private final ScaleGestureDetector.OnScaleGestureListener mScaleGestureListener
                = new ScaleGestureDetector.SimpleOnScaleGestureListener() {

            private float mHeight;

            @Override
            public boolean onScaleBegin(ScaleGestureDetector detector) {
                mIsScaling = true;
                mHeight = mWindowRect.height();
                return super.onScaleBegin(detector);
            }

            @Override
            public void onScaleEnd(ScaleGestureDetector detector) {
                mIsScaling = false;
                super.onScaleEnd(detector);
            }

            @Override
            public boolean onScale(ScaleGestureDetector scaleGestureDetector) {
                float scale = scaleGestureDetector.getScaleFactor();
                mHeight = mHeight * scale;
                if (Math.abs(mHeight - mWindowRect.height()) > MOVE_THRESHOLD) {
                    setSize((int) mHeight);
                }
                return true;
            }
        };

        public void setRGBImageView(Bitmap rgb) {
            if (rgb != null && !rgb.isRecycled()) {
                mColorView.setImageBitmap(rgb);
            }
        }

    }

}
