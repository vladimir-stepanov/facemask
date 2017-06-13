
package grapevine.face1;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

public class MainActivity extends Activity {
    private FaceView mView;

    @Override
    protected void onCreate(Bundle icicle) {
        Log.i("MainActivity:", "onCreate");

        MainLib.onCreate();

        // hide status bar of Android
        // could also be done later
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        AssetManager assetManager = getAssets();
        MainLib.setAssetManager(assetManager);

        super.onCreate(icicle);
        mView = new FaceView(getApplication());
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
    }

    @Override
    protected void onDestroy() {
        Log.i("MainActivity:", "onDestroy");
        super.onDestroy();// Always call the superclass method first
        MainLib.onDestroy();
    }

}
