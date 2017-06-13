
package com.fonttest;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.WindowManager;
import com.fonttest.FontTestView;
import com.fonttest.R;
import com.fonttest.FontTestLib;

public class FontTestActivity extends Activity
{
    FontTestView mView;
    AssetManager assetManager;

    @Override protected void onCreate(Bundle icicle)
    {
        FontTestLib.init();

        // hide statusbar of Android
        // could also be done later
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        assetManager = getAssets();
    	FontTestLib.setAssetManager(assetManager);

        super.onCreate(icicle);
        mView = new FontTestView(getApplication());
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
