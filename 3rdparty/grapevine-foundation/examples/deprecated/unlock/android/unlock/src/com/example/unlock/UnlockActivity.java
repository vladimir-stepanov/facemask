
package com.example.unlock;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.WindowManager;
import com.example.unlock.UnlockView;
import com.example.unlock.R;
import com.example.unlock.UnlockLib;

public class UnlockActivity extends Activity
{
    UnlockView mView;
    AssetManager assetManager;

    @Override protected void onCreate(Bundle icicle)
    {
        UnlockLib.init();

        // hide statusbar of Android
        // could also be done later
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        assetManager = getAssets();
    	UnlockLib.setAssetManager(assetManager);
    	
        super.onCreate(icicle);
        mView = new UnlockView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause()
    {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        mView.onResume();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
    }
}
