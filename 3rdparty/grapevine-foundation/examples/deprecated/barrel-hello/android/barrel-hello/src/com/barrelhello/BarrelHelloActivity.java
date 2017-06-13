
package com.barrelhello;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import com.barrelhello.BarrelHelloView;
import com.barrelhello.R;
import com.barrelhello.BarrelHelloLib;

public class BarrelHelloActivity extends Activity
{
    BarrelHelloView mView;
    AssetManager assetManager;

    @Override protected void onCreate(Bundle icicle)
    {
        BarrelHelloLib.onCreate();

        // hide statusbar of Android
        // could also be done later
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        assetManager = getAssets();
    	BarrelHelloLib.setAssetManager(assetManager);
    	
        super.onCreate(icicle);
        mView = new BarrelHelloView(getApplication());
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
    
    @Override
    protected void onDestroy()
    {
        Log.i("BarrelHelloActivity:", "onDestroy");
        super.onDestroy();// Always call the superclass method first
        BarrelHelloLib.onDestroy();
    }
    
}
