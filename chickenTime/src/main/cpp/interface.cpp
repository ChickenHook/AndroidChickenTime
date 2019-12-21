
#include <jni.h>
#include <android/log.h>
#include <chickenHook/hooking.h>
#include <dlopenBypass/DlOpenBypass.h>
#include <logging/logging.h>
#include <iomanip>
#include <artHelper/ArtPathFinder.h>
#include <invokeHook/ArtMethodInvokeHook.h>



// hooking

JavaVM *_vm;
jclass callbackClass;
jmethodID callbackMethod;


std::string getArtPath(JNIEnv *env) {
    return ArtPathFinder::getArtPath(env);
}
// ArtMethod* ArtMethod::FromReflectedMethod(const ScopedObjectAccessAlreadyRunnable& soa,
//                                          jobject jlr_method)


// _ZNK3art6Thread13DecodeJObjectEP8_jobject

static std::vector<std::string> hooks;

static void addHook(JNIEnv *env, jobject /*this*/, jstring hookMethod, jstring callbackMethod) {
    jboolean isCopy;
    const char *convertedValue = (env)->GetStringUTFChars(hookMethod, &isCopy);
    std::string hookMethodString = std::string(convertedValue);
    hookMethodString.erase(
            std::remove_if(hookMethodString.begin(), hookMethodString.end(), ::isspace),
            hookMethodString.end());
    hooks.push_back(hookMethodString);
    env->ReleaseStringUTFChars(hookMethod, convertedValue);
}

static void propagateToJava(void *method, std::string name) {
    __android_log_print(ANDROID_LOG_DEBUG, "invokeCallback",
                        "trigger java 1 %p", method);
    JNIEnv *env = nullptr;
    if (_vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    if (callbackClass == nullptr) {

        return;
    }
    if (callbackMethod == nullptr) {

        return;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "invokeCallback",
                        "trigger java 2 %p", method);
    //callbackMethod = env->GetStaticMethodID(callbackClass, "onJavaHook", "(Ljava/lang/String;)V");
    jstring functionName = env->NewStringUTF(name.c_str());
    env->CallStaticVoidMethod(callbackClass, callbackMethod, functionName);

}

static void installHooks(
        JNIEnv *env,
        jobject /* this */) {
    // install your hooks here
    //ChickenHook::Hooking::getInstance().hook((void *) &read, (void *) &my_read);
    __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                        "trying to install books");


    void *libart = DlOpenBypass::sys_dlopen(env, getArtPath(env).c_str());
    ArtMethodInvokeHook::getInstance().setCallback([](void *method, std::string name) {
        if (_vm == nullptr) {
            return;
        }
        name.erase(
                std::remove_if(name.begin(), name.end(), ::isspace),
                name.end());
        for (auto hook : hooks) {
            if (hook.find(name) != std::string::npos) {
                propagateToJava(method, name);
            }
        }

    });
    ArtMethodInvokeHook::getInstance().installArtHooks(_vm, libart);
    /*DlOpenBypass::sys_dlopen(env, "art", true, "org/chickenhook/chickenTime/NativeInterface",
                             "dlopenHelper", [](const char *__filename, int __flag) {

                if (__filename != nullptr) {
                    log() << "DlOpenBypass [+] "
                          << "dlopen hook [+] "
                          << " received dlopen of <"
                          << __filename << ">";
                }


                void *res = nullptr;
                ChickenHook::Trampoline trampoline;
                if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
                        (void *) &dlopen,
                        trampoline)) {
                    __android_log_print(ANDROID_LOG_DEBUG,
                                        "stringFromJNI",
                                        "hooked function call original function");
                    log() << "DlOpenBypass [+] "
                          << "dlopen hook [+] "
                          << " call dlopen for <"
                          << __filename << ">";

                    trampoline.copyOriginal();

                    res = dlopen(__filename, __flag);

                    trampoline.reinstall();
                    if (std::string("/system/lib/libart.so") ==
                        __filename) { // use contains instead!!
                        // trigger callback
                        log() << "DlOpenBypass [+] "
                              << "dlopen hook [+] "
                              << " found dlopen of our system library <"
                              << __filename << ">";

                        installArtHooks(res);
                    }
                    return res;
                } else {
                    __android_log_print(ANDROID_LOG_DEBUG,
                                        "stringFromJNI",
                                        "hooked function cannot call original function");
                }
                return (void *) nullptr;
            });*/
    /*invokeReflectionMethodFunction = (jobject (*)(const ScopedObjectAccessAlreadyRunnable &,
                                                  jobject, jobject,
                                                  jobject, bool)) dlsym(libart,
                                                                        "_ZN3art12InvokeMethodERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectS4_S4_b");
    if (invokeReflectionMethodFunction) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                            "Install hook: _ZN3art12InvokeMethodERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectS4_S4_b");
        ChickenHook::Hooking::getInstance().hook((void *)
                                                         invokeReflectionMethodFunction,
                                                 (void *) &my_ZN3art12InvokeMethodERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectS4_S4_b);
    }*/

}


// JNI Stuff
static const char *classPathName = "org/chickenhook/chickenTime/NativeInterface";


static const JNINativeMethod gMethods[] = {
        {"installHooks", "()V",                                     (void *) installHooks},
        {"addHook",      "(Ljava/lang/String;Ljava/lang/String;)V", (void *) addHook}
};

void getGlobalRef(JNIEnv *jenv, const char *clazz, jclass *globalClass) {
    jclass local = (jenv)->FindClass(clazz);
    if (local) {
        *globalClass = (jclass) (jenv)->NewGlobalRef(local);
        (jenv)->DeleteLocalRef(local);
    }
}

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    getGlobalRef(env, className, &callbackClass);
    if (callbackClass == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    callbackMethod = env->GetStaticMethodID(callbackClass, "onJavaHook", "(Ljava/lang/String;)V");
    if (callbackMethod == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to find callback method '%s'",
                            "onJavaHook");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(callbackClass, gMethods, numMethods) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "registerNativeMethods",
                            "Native registration unable to register natives...");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
    _vm = vm;
    JNIEnv *env = nullptr;
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