package com.hfs.furyclient.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import com.hfs.furyclient.R;

public class HFSControlView extends RelativeLayout implements View.OnClickListener {
    private Context mContext;
    private HFSControlViewListener mListener;
    private ImageView mCloseButton;
    private ImageView mPreviousStepButton;
    private ImageView mNextStepButton;
    private ImageView mArHintButton;

    public HFSControlView(Context context) {
        super(context);
        mContext = context;
        init();
    }

    public HFSControlView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        init();
    }

    public HFSControlView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        init();
    }

    public HFSControlView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        init();
    }

    /**
     * Initializes the view.
     */
    private void init() {
        View view = LayoutInflater.from(mContext).inflate(
                R.layout.instruction_control_view, this, true);
        mCloseButton = view.findViewById(R.id.instruction_button_close);
        mNextStepButton = view.findViewById(R.id.instruction_button_next_step);
        mPreviousStepButton = view.findViewById(R.id.instruction_button_previous_step);
        mArHintButton = view.findViewById(R.id.instruction_button_ar_hint);
        mCloseButton.setOnClickListener(this);
        mNextStepButton.setOnClickListener(this);
        mPreviousStepButton.setOnClickListener(this);
        mArHintButton.setOnClickListener(this);
    }

    public void enablePrevious(boolean enable) {
        mPreviousStepButton.setEnabled(enable);
    }

    @Override
    public void onClick(View view) {
        if (mListener != null) {
            switch (view.getId()) {
                case R.id.instruction_button_close:
                    mListener.OnCloseClick();
                    break;
                case R.id.instruction_button_next_step:
                    mListener.OnNextStepClick();
                    break;
                case R.id.instruction_button_previous_step:
                    mListener.OnPreviousStepClick();
                    break;
                case R.id.instruction_button_ar_hint:
                    mListener.OnArHintClick();
                    break;
            }
        }
    }

    /**
     * Sets the buttons listener.
     *
     * @param listener
     */
    public void setListener(HFSControlViewListener listener) {
        mListener = listener;
    }

    /**
     * Buttons click listener.
     */
    public interface HFSControlViewListener {
        /**
         * On close button click.
         */
        void OnCloseClick();

        /**
         * On next step button click.
         */
        void OnNextStepClick();

        /**
         * On previous step button click.
         */
        void OnPreviousStepClick();

        /**
         * On Ar Hint button click.
         */
        void OnArHintClick();
    }
}
