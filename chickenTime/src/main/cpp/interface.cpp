
#include <jni.h>
#include <android/log.h>
#include <chickenHook/hooking.h>

// hooking
ssize_t my_read(int __fd, void *__buf, size_t __count) {
    __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI", "read called [-] %d", __fd);

    int res = -1;
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr((void *) &read, trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function call original function");
        // printLines(hexdump(static_cast<const uint8_t *>(__buf), __count, "read"));
        trampoline.copyOriginal();
        res = read(__fd, __buf, __count);
        trampoline.reinstall();
        return res;
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "stringFromJNI",
                            "hooked function cannot call original function");
    }
    return res;
}


static void installHooks(
        JNIEnv *env,
        jobject /* this */) {
    // install your hooks here
    ChickenHook::Hooking::getInstance().hook((void *) &read, (void *) &my_read);

}


// JNI Stuff
static const char *classPathName = "org/chickenhook/chickenTime/NativeInterface";


static const JNINativeMethod gMethods[] = {
        {"installHooks", "()V", (void *) installHooks},
};

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to register natives...");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    ChickenHook::Hooking::getInstance().setLoggingCallback([](const std::string message) {
        __android_log_print(ANDROID_LOG_DEBUG, "ChickenHook", "%s", message.c_str());
    });


    if (!registerNativeMethods(env, classPathName,
                               (JNINativeMethod *) gMethods,
                               sizeof(gMethods) / sizeof(gMethods[0]))) {
        return -1;
    }

    return JNI_VERSION_1_4;
}