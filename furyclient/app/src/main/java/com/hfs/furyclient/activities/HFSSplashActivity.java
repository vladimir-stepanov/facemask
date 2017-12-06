package com.hfs.furyclient.activities;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;

import com.hfs.furyclient.R;

/**
 * Created by hfs on 30.11.2017.
 */

public class HFSSplashActivity extends AppCompatActivity {
    private final int mStartDelay = 2000;
    private Handler mStartHandler = new Handler();
    private Runnable mStartRunnable = new Runnable() {
        @Override
        public void run() {
            start();
        }
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
        mStartHandler.postDelayed(mStartRunnable, mStartDelay);
    }

    private void start() {
        startActivity(new Intent(HFSSplashActivity.this, HFSMainActivity.class));
        finish();
    }
}
