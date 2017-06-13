
package grapevine.face1;

import android.content.res.AssetManager;

// Wrapper for native library

public class MainLib {
    static {
        System.loadLibrary("grapevine-foundation");
        System.loadLibrary("grapevine-3dengine");
        System.loadLibrary("native-lib");
    }

    public static native void onCreate();

    public static native void onDestroy();

    /**
     * @param width  the current view width
     * @param height the current view height
     */
    public static native void surfaceChanged(int width, int height);

    public static native void step(boolean haveFace, boolean newPose, float px, float py, float pz, float[] tmpModelView, float[] frustumScale, float[] landmarks);

    public static native void touch(float x, float y, float dx, float dy, int flags);

    public static native void setAssetManager(AssetManager assetManager);
}
