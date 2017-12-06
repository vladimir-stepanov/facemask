package com.hfs.furyclient.views;

import android.content.Context;
import android.graphics.Typeface;
import android.support.annotation.Nullable;
import android.util.AttributeSet;

/**
 * Created by hfs on 30.11.2017.
 */

public class HFSTextView extends android.support.v7.widget.AppCompatTextView {
    public HFSTextView(Context context) {
        super(context);
        init();
    }

    public HFSTextView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public HFSTextView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init(){
        Typeface tf = Typeface.createFromAsset(
                getContext().getAssets(), "fonts/ProductSans.otf");
        setTypeface(tf, Typeface.NORMAL);
    }
}
