package com.huawei.facemask;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.UiThread;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.lang.ref.WeakReference;

class FloatingPreviewWindow {

    static final int DEFAULT_SCALE = 25;
    private final Context mContext;
    private final Handler mUIHandler;
    private WindowManager.LayoutParams mWindowParam;
    private WindowManager mWindowManager;
    private FloatCamView mRootView;
    private int mWindowWidth;
    private int mWindowHeight;

    FloatingPreviewWindow(Context context) {
        mContext = context;
        mUIHandler = new Handler(Looper.getMainLooper());
        Point size = new Point();
        Display display = ((WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealSize(size);
        int screenWidth = size.x;
        int screenHeight = size.y;

        // Default window size
        mWindowWidth = screenWidth * DEFAULT_SCALE / 100;
        mWindowHeight = screenHeight * DEFAULT_SCALE / 100;

        mWindowWidth = mWindowWidth > 0 && mWindowWidth < screenWidth ? mWindowWidth : screenWidth;
        mWindowHeight = mWindowHeight > 0 && mWindowHeight < screenHeight ? mWindowHeight : screenHeight;
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

    void setSize(int width, int height) {
        if (mRootView != null) {
            mRootView.setSize(width, height);
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

        mWindowParam.alpha = 1.0f;

        mWindowParam.gravity = Gravity.TOP | Gravity.START;
        mWindowParam.x = 0;
        mWindowParam.y = 0;
        mWindowParam.width = mWindowWidth;
        mWindowParam.height = mWindowHeight;
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
        private static final int MOVE_THRESHOLD = 10;
        private final WeakReference<FloatingPreviewWindow> mWeakRef;
        private final LayoutInflater mLayoutInflater;
        private final ImageView mColorView;
        private int mLastX;
        private int mLastY;
        private int mFirstX;
        private int mFirstY;
        private boolean mIsMoving = false;

        public FloatCamView(FloatingPreviewWindow window) {
            super(window.mContext);
            mWeakRef = new WeakReference<>(window);
            mLayoutInflater = (LayoutInflater) window.mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            FrameLayout body = this;
            body.setOnTouchListener(new OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    return false;
                }
            });

            mLayoutInflater.inflate(R.layout.cam_window_view, body, true);
            mColorView = (ImageView) findViewById(R.id.preview);

            setSize(mWindowWidth, mWindowHeight);
        }

        public void setSize(int width, int height) {
            mColorView.getLayoutParams().width = width;
            mColorView.getLayoutParams().height = height;
            WindowManager windowMgr = mWeakRef.get().mWindowManager;
            WindowManager.LayoutParams params = mWeakRef.get().mWindowParam;
            if (params != null) {
                params.width = width;
                params.height = height;
                windowMgr.updateViewLayout(this, params);
            }
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
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
                        WindowManager windowMgr = mWeakRef.get().mWindowManager;
                        WindowManager.LayoutParams params = mWeakRef.get().mWindowParam;
                        if (event.getPointerCount() == 1 && windowMgr != null) {
                            params.x -= deltaX;
                            params.y -= deltaY;
                            windowMgr.updateViewLayout(this, params);
                        }
                    }
                    break;

                case MotionEvent.ACTION_UP:
                    mIsMoving = false;
                    break;
            }
            return true;
        }

        public void setRGBImageView(Bitmap rgb) {
            if (rgb != null && !rgb.isRecycled()) {
                mColorView.setImageBitmap(rgb);
            }
        }

    }

}
