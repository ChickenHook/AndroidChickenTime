package org.chickenhook.chickenTime;

public class NativeInterface {
    static {
        System.loadLibrary("ChickenTime");
    }

    public static native void installHooks();
}
