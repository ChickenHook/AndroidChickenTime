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

    private static void onJavaHook(String method) {

        Log.d("NativeInterface", "onJavaHook [-] callback triggered <" + method + ">");
        if (callback != null) {
            callback.onArtMethodInvoked(method, method);
        }
        //String stackTrace = Log.getStackTraceString(new RuntimeException());
        //Log.d("NativeInterface", "onJavaHook [-] stackTrace " + stackTrace);

    }

    public static native void installHooks();

    public static void addHook(Method hook, Method callback) {
        Log.d("NativeInterface", "addHook [-] triggered <" + hook.toGenericString() + "." + callback.toGenericString() + ">");
        addHook(hook.toGenericString(), callback.toGenericString());

    }

    public static native void addHook(String hook, String callback);

    public interface OnArtMethodInvoked {
        void onArtMethodInvoked(String clazz, String name);
    }
}
