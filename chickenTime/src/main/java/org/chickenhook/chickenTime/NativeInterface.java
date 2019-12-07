package org.chickenhook.chickenTime;

import android.util.Log;

import androidx.annotation.NonNull;

import java.lang.reflect.Method;

public class NativeInterface {
    static {
        System.loadLibrary("ChickenTime");
    }

    private volatile static OnArtMethodInvoked callback = null;

    public static void setCallback(@NonNull OnArtMethodInvoked onArtMethodInvoked) {
        callback = onArtMethodInvoked;
    }

    private static void onJavaHook() {
        int index = 3;
        if (Thread.currentThread().getStackTrace().length < 4) {
            return;
        }
        StackTraceElement element = Thread.currentThread().getStackTrace()[index];
        Log.d("NativeInterface", "onJavaHook [-] callback triggered <" + element.getClassName() + "." + element.getMethodName() + ">");
        if (callback != null) {
            callback.onArtMethodInvoked(element.getClassName(), element.getMethodName());
        }
        //String stackTrace = Log.getStackTraceString(new RuntimeException());
        //Log.d("NativeInterface", "onJavaHook [-] stackTrace " + stackTrace);

    }

    public static native void installHooks();

    public static native void addHook(Method hook, Method callback);

    public interface OnArtMethodInvoked {
        void onArtMethodInvoked(String clazz, String name);
    }
}
