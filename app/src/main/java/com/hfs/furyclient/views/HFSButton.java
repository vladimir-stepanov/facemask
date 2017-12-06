package com.hfs.furyclient.views;

import android.content.Context;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.widget.Button;

/**
 * Created by hfs on 01.12.2017.
 */

public class HFSButton extends Button {
    public HFSButton(Context context) {
        super(context);
        init();
    }

    public HFSButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public HFSButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        Typeface tf = Typeface.createFromAsset(
                getContext().getAssets(), "fonts/ProductSans.otf");
        setTypeface(tf, Typeface.NORMAL);
    }
}
