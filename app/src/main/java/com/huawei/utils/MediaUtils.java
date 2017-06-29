package com.huawei.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.media.MediaMetadataRetriever;
import android.util.Log;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

public class MediaUtils {

    public static final int HORIZONTAL_SIZE_THUMBNAIL = 250;
    public static final int VERTICAL_SIZE_THUMBNAIL = 250;
    private final static String TAG = "MediaUtils";
    private static final int FILE_TYPE_UNKNOWN = 0;
    // Video file types
    private static final int FILE_TYPE_MP4 = 21;
    private static final int FILE_TYPE_M4V = 22;
    private static final int FILE_TYPE_3GPP = 23;
    private static final int FILE_TYPE_3GPP2 = 24;
    private static final int FILE_TYPE_ASF = 25;
    private static final int FIRST_VIDEO_FILE_TYPE = FILE_TYPE_MP4;
    private static final int LAST_VIDEO_FILE_TYPE = FILE_TYPE_ASF;
    // Image file types
    private static final int FILE_TYPE_JPEG = 31;
    private static final int FILE_TYPE_GIF = 32;
    private static final int FILE_TYPE_PNG = 33;
    private static final int FILE_TYPE_BMP = 34;
    private static final int FILE_TYPE_WBMP = 35;
    private static final int FIRST_IMAGE_FILE_TYPE = FILE_TYPE_JPEG;
    private static final int LAST_IMAGE_FILE_TYPE = FILE_TYPE_WBMP;
    private static final Map<String, Integer> sFileTypeMap = new HashMap<>();
    /* Options used internally. */
    private static final int OPTIONS_SCALE_UP = 0x1;
    private static final int OPTIONS_RECYCLE_INPUT = 0x2;

    static {

        addFileType("MP4", FILE_TYPE_MP4);
        addFileType("M4V", FILE_TYPE_M4V);
        addFileType("3GP", FILE_TYPE_3GPP);
        addFileType("3GPP", FILE_TYPE_3GPP);
        addFileType("3G2", FILE_TYPE_3GPP2);
        addFileType("3GPP2", FILE_TYPE_3GPP2);

        addFileType("JPG", FILE_TYPE_JPEG);
        addFileType("JPEG", FILE_TYPE_JPEG);
        addFileType("GIF", FILE_TYPE_GIF);
        addFileType("PNG", FILE_TYPE_PNG);
        addFileType("BMP", FILE_TYPE_BMP);
        addFileType("WBMP", FILE_TYPE_WBMP);

    }

    private MediaUtils() {
    }

    private static void addFileType(String extension, int fileType) {
        sFileTypeMap.put(extension, fileType);
    }

    private static int getFileType(String path) {
        final int lastDot = path.lastIndexOf('.');
        if (lastDot < 0) {
            return FILE_TYPE_UNKNOWN;
        }
        final Integer result =
                sFileTypeMap.get(path.substring(lastDot + 1).toUpperCase(Locale.getDefault()));
        if (result == null) {
            return FILE_TYPE_UNKNOWN;
        }
        return result;
    }

    public static boolean isVideoFile(String fileName) {
        final int fileType = getFileType(fileName);
        return (fileType >= FIRST_VIDEO_FILE_TYPE && fileType <= LAST_VIDEO_FILE_TYPE);
    }

    public static boolean isImageFile(String fileName) {
        final int fileType = getFileType(fileName);
        return (fileType >= FIRST_IMAGE_FILE_TYPE && fileType <= LAST_IMAGE_FILE_TYPE);
    }

    /**
     * Create a video thumbnail for a video. May return null if the video is
     * corrupt or the format is not supported.
     *
     * @param filePath the path of video file
     */
    public static Bitmap createMovieThumbnail(String filePath) {
        Bitmap bitmap;
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        retriever.setDataSource(filePath);
        bitmap = retriever.getFrameAtTime();
        retriever.release();
        return extractThumbnail(bitmap, OPTIONS_RECYCLE_INPUT);
    }

    /**
     * This method first examines if the thumbnail embedded in EXIF is bigger
     * than our target size. If not, then it'll create a thumbnail from original
     * image.
     *
     * @param filePath the path of image file
     * @return Bitmap
     */
    @SuppressWarnings("deprecation")
    public static Bitmap createImageThumbnail(String filePath) {
        final SizedThumbnailBitmap sizedThumbnailBitmap = new SizedThumbnailBitmap();
        Bitmap bitmap = null;
        if (getFileType(filePath) == FILE_TYPE_JPEG) {
            createThumbnailFromEXIF(filePath, sizedThumbnailBitmap);
            bitmap = sizedThumbnailBitmap.mBitmap;
        }

        if (bitmap == null) {
            FileInputStream fStream = null;
            try {
                fStream = new FileInputStream(filePath);
                final FileDescriptor fDescriptor = fStream.getFD();
                final BitmapFactory.Options options = new BitmapFactory.Options();
                options.inSampleSize = 1;
                options.inJustDecodeBounds = true;
                BitmapFactory.decodeFileDescriptor(fDescriptor, null, options);
                if (options.mCancel || options.outWidth == -1 || options.outHeight == -1) {
                    return null;
                }
                options.inPreferredConfig = Bitmap.Config.ARGB_8888;
                options.inJustDecodeBounds = false;
                options.inDither = false;
                if (options.outWidth > options.outHeight) {
                    options.inSampleSize = options.outWidth / HORIZONTAL_SIZE_THUMBNAIL;
                } else {
                    options.inSampleSize = options.outHeight / VERTICAL_SIZE_THUMBNAIL;
                }
                if (getFileType(filePath) == FILE_TYPE_GIF) {
                    bitmap = BitmapFactory.decodeFile(filePath, options);
                } else {
                    bitmap = BitmapFactory.decodeFileDescriptor(fDescriptor, null, options);
                }
            } catch (Exception ex) {
                Log.e(TAG, ex.toString());
            } finally {
                if (fStream != null) {
                    try {
                        fStream.close();
                    } catch (IOException ignored) {
                    }
                }
            }
        }

        return extractThumbnail(bitmap, OPTIONS_RECYCLE_INPUT);
    }

    @SuppressWarnings("SameParameterValue")
    private static Bitmap extractThumbnail(Bitmap source, int options) {
        if (source == null) {
            return null;
        }
        final int width = source.getWidth();
        final int height = source.getHeight();
        if (width <= HORIZONTAL_SIZE_THUMBNAIL && height <= VERTICAL_SIZE_THUMBNAIL) {
            return source;
        }
        int outWidth;
        int outHeight;
        if (width > height) {
            outWidth = HORIZONTAL_SIZE_THUMBNAIL;
            outHeight = (int) (height * (HORIZONTAL_SIZE_THUMBNAIL / (double) width));
        } else {
            outWidth = (int) (width * (VERTICAL_SIZE_THUMBNAIL / (double) height));
            outHeight = VERTICAL_SIZE_THUMBNAIL;
        }
        float scale;
        if (outWidth < outHeight) {
            scale = (float) (width / (double) outWidth);
        } else {
            scale = (float) (height / (double) outHeight);
        }
        final Matrix matrix = new Matrix();
        matrix.setScale(scale, scale);
        return transform(matrix, source, outWidth, outHeight, OPTIONS_SCALE_UP | options);
    }

    /**
     * Transform source Bitmap to targeted width and height.
     */
    private static Bitmap transform(Matrix scaler, Bitmap source, int targetWidth,
                                    int targetHeight, int options) {
        final boolean scaleUp = (options & OPTIONS_SCALE_UP) != 0;
        final boolean recycle = (options & OPTIONS_RECYCLE_INPUT) != 0;

        final int deltaX = source.getWidth() - targetWidth;
        final int deltaY = source.getHeight() - targetHeight;
        if (!scaleUp && (deltaX < 0 || deltaY < 0)) {
            /*
             * In this case the bitmap is smaller, at least in one dimension,
             * than the target. Transform it by placing as much of the image as
             * possible into the target and leaving the top/bottom or left/right
             * (or both) black.
             */
            final Bitmap bmp2 = Bitmap.createBitmap(targetWidth, targetHeight, Bitmap.Config.ARGB_8888);
            final Canvas canvas = new Canvas(bmp2);

            final int deltaXHalf = Math.max(0, deltaX / 2);
            final int deltaYHalf = Math.max(0, deltaY / 2);
            final Rect src =
                    new Rect(deltaXHalf, deltaYHalf, deltaXHalf
                            + Math.min(targetWidth, source.getWidth()), deltaYHalf
                            + Math.min(targetHeight, source.getHeight()));
            final int dstX = (targetWidth - src.width()) / 2;
            final int dstY = (targetHeight - src.height()) / 2;
            final Rect dst = new Rect(dstX, dstY, targetWidth - dstX, targetHeight - dstY);
            canvas.drawBitmap(source, src, dst, null);
            if (recycle) {
                source.recycle();
            }
            return bmp2;
        }
        final double bitmapWidthF = source.getWidth();
        final double bitmapHeightF = source.getHeight();

        final float bitmapAspect = (float) (bitmapWidthF / bitmapHeightF);
        final float viewAspect = (float) (targetWidth / (double) targetHeight);

        Bitmap bmp1 = null;
        if (bitmapAspect > viewAspect) {
            final float scale = (float) (targetHeight / bitmapHeightF);
            if (scale < .9F || scale > 1F) {
                scaler.setScale(scale, scale);
            } else {
                bmp1 = source;
            }
        } else {
            final float scale = (float) (targetWidth / bitmapWidthF);
            if (scale < .9F || scale > 1F) {
                scaler.setScale(scale, scale);
            } else {
                bmp1 = source;
            }
        }

        if (bmp1 == null) {
            // this is used for minithumb and crop, so we want to filter here.
            bmp1 =
                    Bitmap.createBitmap(source, 0, 0, source.getWidth(), source.getHeight(),
                            scaler, true);
        }

        if (recycle && !bmp1.equals(source)) {
            source.recycle();
        }

        final int dx1 = Math.max(0, bmp1.getWidth() - targetWidth);
        final int dy1 = Math.max(0, bmp1.getHeight() - targetHeight);

        final Bitmap bmp2 = Bitmap.createBitmap(bmp1, dx1 / 2, dy1 / 2, targetWidth, targetHeight);

        if (!bmp2.equals(bmp1) && (recycle || !bmp1.equals(source))) {
            bmp1.recycle();
        }

        return bmp2;
    }

    /**
     * Creates a bitmap by either downsampling from the thumbnail in EXIF or the
     * full image. The functions returns a SizedThumbnailBitmap, which contains
     * a downsampled bitmap and the thumbnail data in EXIF if exists.
     */
    private static void createThumbnailFromEXIF(String filePath, SizedThumbnailBitmap sizedThumbBitmap) {
        if (filePath == null) {
            return;
        }
        final BitmapFactory.Options fullOptions = new BitmapFactory.Options();
        fullOptions.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(filePath, fullOptions);
        if (fullOptions.outWidth > fullOptions.outHeight) {
            fullOptions.inSampleSize = fullOptions.outWidth / HORIZONTAL_SIZE_THUMBNAIL;
        } else {
            fullOptions.inSampleSize = fullOptions.outHeight / VERTICAL_SIZE_THUMBNAIL;
        }
        fullOptions.inJustDecodeBounds = false;
        sizedThumbBitmap.mBitmap = BitmapFactory.decodeFile(filePath, fullOptions);
    }

    /**
     * SizedThumbnailBitmap contains the bitmap, which is downsampled either
     * from the thumbnail in exif or the full image.
     */
    private static class SizedThumbnailBitmap {
        Bitmap mBitmap;
    }

}
