package com.hfs.furyclient.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

import com.hfs.furyclient.R;

import java.util.ArrayList;
import java.util.List;

public class HFSDrawingView extends View {
    public static float scale;
    private List<Rect> mObjects;
    private Paint mPaint;
    private Point mStartPoint;
    private Point mEndPoint;
    private Point mArrowPoint1;
    private Point mArrowPoint2;

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
        if (mStartPoint != null && mEndPoint != null) {
            mPaint.setStrokeWidth(15.0f);
            canvas.drawLine(mStartPoint.x, mStartPoint.y, mEndPoint.x, mEndPoint.y, mPaint);
            canvas.drawLine(mEndPoint.x, mEndPoint.y, mArrowPoint1.x, mArrowPoint1.y, mPaint);
            canvas.drawLine(mEndPoint.x, mEndPoint.y, mArrowPoint2.x, mArrowPoint2.y, mPaint);
        }
        for (Rect rect : mObjects) {
            mPaint.setStrokeWidth(5.0f);
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
        mPaint.setStrokeCap(Paint.Cap.ROUND);
        mPaint.setColor(getResources().getColor(R.color.hintArrowColor));
        mStartPoint = new Point();
        mEndPoint = new Point();
        mArrowPoint1 = new Point();
        mArrowPoint2 = new Point();
    }

    public void updateObjects(List<Rect> objects) {
        if (objects != null && objects.size() >= 2) {
            mObjects = objects;
            Rect rect1 = objects.get(0);
            Rect rect2 = objects.get(1);
            if (mStartPoint == null) {
                mStartPoint = new Point();
            }
            if (mEndPoint == null) {
                mEndPoint = new Point();
            }
            mStartPoint.set((int) (rect1.centerX() * scale), (int) (rect1.centerY() * scale));
            mEndPoint.set((int) (rect2.centerX() * scale), (int) (rect2.centerY() * scale));

            float deltaX = (mStartPoint.x - mEndPoint.x) / 5;
            float deltaY = (mStartPoint.y - mEndPoint.y) / 5;
            float frac = 0.3f;
            mArrowPoint1.set(
                    (int) (mEndPoint.x + ((1 - frac) * deltaX + frac * deltaY)),
                    (int) (mEndPoint.y + ((1 - frac) * deltaY - frac * deltaX)));
            mArrowPoint2.set(
                    (int) (mEndPoint.x + ((1 - frac) * deltaX - frac * deltaY)),
                    (int) (mEndPoint.y + ((1 - frac) * deltaY + frac * deltaX)));
        } else {
            mStartPoint = null;
            mEndPoint = null;
            mObjects.clear();
        }
        postInvalidate();
    }
}
