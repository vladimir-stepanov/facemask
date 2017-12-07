package com.hfs.furyclient.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by hfs on 07.12.2017.
 */

public class HFSDrawingView extends View {
    private static final float scale = 2.0f;
    private List<Rect> mObjects;
    private Paint mPaint;

    public HFSDrawingView(Context context) {
        super(context);
        init();
    }

    public HFSDrawingView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public HFSDrawingView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public HFSDrawingView(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        for (Rect rect : mObjects) {
            canvas.drawRect(
                    rect.left * scale,
                    rect.top * scale,
                    rect.right * scale,
                    rect.bottom * scale,
                    mPaint);
        }
    }

    private void init() {
        mObjects = new ArrayList<>();
        mPaint = new Paint();
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(20.0f);
        mPaint.setColor(Color.RED);
    }

    public void updateObjects(List<Rect> objects) {
        mObjects = objects;
        postInvalidate();
    }
}
