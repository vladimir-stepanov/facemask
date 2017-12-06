package com.hfs.furyclient.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.hfs.furyclient.R;

/**
 * Created by hfs on 01.12.2017.
 */

public class HFSInstructionHintView extends RelativeLayout implements View.OnClickListener {
    private Context mContext;
    private String mHint;
    private TextView mHintTextView;
    private Button mGotItButton;
    private HFSHintViewClickListener mListener;

    public HFSInstructionHintView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    public HFSInstructionHintView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        init();
    }

    public HFSInstructionHintView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        init();
    }

    public HFSInstructionHintView(Context context) {
        super(context);
        mContext = context;
        init();
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.instruction_hint_button_got_it:
                if (mListener != null) {
                    mListener.OnGotItClick();
                }
                break;
        }
    }

    /**
     * Initializes the view.
     */
    private void init() {
        View view = LayoutInflater.from(mContext).inflate(
                R.layout.instruction_hint_layout, this, true);
        mHintTextView = view.findViewById(R.id.instruction_hint_text_view);
        mHintTextView.setText(mHint);
        mGotItButton = view.findViewById(R.id.instruction_hint_button_got_it);
        mGotItButton.setOnClickListener(this);
    }

    /**
     * Sets the buttons listener.
     *
     * @param listener
     */
    public void setListener(HFSHintViewClickListener listener) {
        mListener = listener;
    }

    /**
     * Sets the hint text.
     *
     * @param hint
     */
    public void updateHintText(String hint) {
        mHint = hint;
        mHintTextView.setText(mHint);
    }

    /**
     * Buttons click listener.
     */
    public interface HFSHintViewClickListener {
        /**
         * On got it button click.
         */
        void OnGotItClick();
    }
}
