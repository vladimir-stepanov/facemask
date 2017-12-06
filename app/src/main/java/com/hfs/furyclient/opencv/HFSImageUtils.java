package com.hfs.furyclient.opencv;

/**
 * Created by EStepanova on 06.12.2017.
 */

public class HFSImageUtils {
    /**
     * Converts YUV420 to ARGB8888
     *
     * @param y
     * @param u
     * @param v
     * @param output
     * @param width
     * @param height
     * @param yRowStride
     * @param uvRowStride
     * @param uvPixelStride
     */
    public static native void convertYUV420ToARGB8888(
            byte[] y,
            byte[] u,
            byte[] v,
            int[] output,
            int width,
            int height,
            int yRowStride,
            int uvRowStride,
            int uvPixelStride);
}
