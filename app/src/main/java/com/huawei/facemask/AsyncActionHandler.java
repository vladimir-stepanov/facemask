package com.huawei.facemask;

import android.os.Handler;
import android.os.HandlerThread;

final class AsyncActionHandler {
    private static final HandlerThread sHandlerThread = new HandlerThread("AsyncActionHandler");
    private static final Handler sHandler;

    static {
        sHandlerThread.start();
        sHandler = new Handler(sHandlerThread.getLooper());
    }

    private AsyncActionHandler() {
    }

    static void post(Runnable runnable) {
        sHandler.post(runnable);
    }
}
