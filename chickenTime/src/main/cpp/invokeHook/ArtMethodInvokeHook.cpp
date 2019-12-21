//
// Created by Sascha Roth on 07.12.19.
//

#include <android/log.h>
#include <dlfcn.h>
#include <chickenHook/hooking.h>
#include <jni.h>
#include <logging/logging.h>
#include <iomanip>
#include "ArtMethodInvokeHook.h"



/////////////// convert

static inline jmethodID EncodeArtMethod(void *art_method) {
    return reinterpret_cast<jmethodID>(art_method);
}

static inline void *DecodeArtMethod(jmethodID method_id) {
    return reinterpret_cast<void *>(method_id);
}



/////////////// convert

void *artMethodInvoke = nullptr; // 19 const char *shorty
extern JavaVM *_vm;

void (*ArtMethodInvokeHook::_callback)(void *, std::string name) = (void (*)(void *,
                                                                             std::string name)) nullptr;

void ArtMethodInvokeHook::installArtHooks(JavaVM *vm, void *libart) {
    if (libart == nullptr) {
        __android_log_print(ANDROID_LOG_DEBUG, "installArtHooks",
                            "given libart handle is null :(");
        return;
    }
    _vm = vm;

    // art method invoke
    artMethodInvoke = dlsym(libart,
                            "_ZN3art9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc");

    if (artMethodInvoke) {
        __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                            "Install hook: _ZN3art9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc");
        ChickenHook::Hooking::getInstance().hook(artMethodInvoke,
                                                 (void *) &myArtMethodInvoke27);

    } else {
        artMethodInvoke = dlsym(libart,
                                "_ZN3art6mirror9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc");
        if (artMethodInvoke) {
            __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                                "Install hook: _ZN3art6mirror9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc");
            ChickenHook::Hooking::getInstance().hook(artMethodInvoke,
                                                     (void *) &myArtMethodInvoke22);
        } else {
            artMethodInvoke = dlsym(libart,
                                    "_ZN3art6mirror9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEc");
            if (artMethodInvoke) {
                __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                                    "Install hook: _ZN3art6mirror9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEc");
                ChickenHook::Hooking::getInstance().hook(artMethodInvoke,
                                                         (void *) &myArtMethodInvoke19);
            } else {

            }

        }
    }
    bool prettyHooksResult = artMethodPrettyName.installHooks(vm, libart);

    __android_log_print(ANDROID_LOG_DEBUG, "installHooks",
                        "artMethodInvoke <%p> artMethodGetName <%d>", artMethodInvoke,
                        prettyHooksResult);
}


// helpers



std::string hexdump(const uint8_t *buffer, std::size_t size, const std::string &title = "HEXDUMP") {
    using namespace std;
    ostringstream ost;
    ost << title << '\n';
    const auto bytes = vector<uint8_t>(buffer, buffer + size);
    std::string chars;

    for (auto p = cbegin(bytes); p < cend(bytes);) {
        constexpr auto ptrSize = sizeof(void *);
        constexpr auto byteSize = sizeof(uint8_t);
        constexpr auto wordsPerRow = 4;
        const auto bytesToGo = static_cast<unsigned>(std::distance(p, cend(bytes)));
        const auto nBytes = std::min((unsigned) ptrSize, bytesToGo);
        const auto bytesPrinted = static_cast<unsigned>(std::distance(cbegin(bytes), p));
        const auto isFirstRow = bytesPrinted == 0;
        const auto isNewRow = (bytesPrinted % (ptrSize * wordsPerRow)) == 0;
        const auto isLastRow = (p + nBytes) == cend(bytes);

        if (isNewRow && !isLastRow) {
            if (!isFirstRow) {
                ost << '\n';
            }
            ost << hex << setw(ptrSize * 2) << setfill('0') << bytesPrinted << "   ";
        }

        {
            for_each(p, p + nBytes, [&ost, &chars](uint8_t byte) {
                ost << ' ' << hex << setw(byteSize * 2) << setfill('0')
                    << static_cast<unsigned>(byte);
                chars += (char) (byte);
            });
            ost << "   ";
        }

        if (isLastRow) {
            ost << chars << '\n';
        }

        p += nBytes;
    }
    return ost.str();
}

void printMethodName(jobject obj) {
    JNIEnv *env = nullptr;
    if (_vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    if (_vm == nullptr) {
        return;
    }
    jclass cls = env->GetObjectClass(obj);

// First get the class object
    jmethodID mid = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
    jstring strObj = (jstring) env->CallObjectMethod(obj, mid);

// Now get the c string from the java jstring object
    const char *str = env->GetStringUTFChars(strObj, NULL);

// Print the class name
    __android_log_print(ANDROID_LOG_DEBUG,
                        "printMethodName", "Calling method is: %s", str);

// Release the memory pinned char array
    env->ReleaseStringUTFChars(strObj, str);
}

void printClassName(jobject obj) {
    JNIEnv *env = nullptr;
    if (_vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    if (_vm == nullptr) {
        return;
    }
    jclass cls = env->GetObjectClass(obj);

// First get the class object
    jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
    jobject clsObj = env->CallObjectMethod(obj, mid);

// Now get the class object's class descriptor
    cls = env->GetObjectClass(clsObj);

// Find the getName() method on the class object
    mid = env->GetMethodID(cls, "getName", "()Ljava/lang/String;");

// Call the getName() to get a jstring object back
    jstring strObj = (jstring) env->CallObjectMethod(clsObj, mid);

// Now get the c string from the java jstring object
    const char *str = env->GetStringUTFChars(strObj, nullptr);

// Print the class name
    __android_log_print(ANDROID_LOG_DEBUG,
                        "printClassName", "Calling class is: %s", str);

// Release the memory pinned char array
    env->ReleaseStringUTFChars(strObj, str);
}


void parseArgs(char *args, uint32_t args_size, char const *shorty_str) {

    std::string hex = hexdump(reinterpret_cast<const uint8_t *>(args), args_size, "parseArgs");
    log() << hex;
    uint32_t *int_val;
    float *float_val;
    jobject *obj;
    double *double_val;
    uint64_t *long_val;
    std::string shorty(shorty_str);
    for (int i = 0, offset = 0; i < shorty.size(); ++i) {
        switch (shorty[i]) {
            case 'Z':
            case 'B':
            case 'C':
            case 'S':
            case 'I':
                int_val = (uint32_t *) (&args[offset]);
                offset += 4;
                break;
            case 'F':
                float_val = (float *) (&args[offset]);
                offset += 4;
                break;
            case 'L': {
                obj = (jobject *) (&args[offset]);  // TODO: local reference
                printClassName(*obj);
                offset += sizeof(void *);
                break;
            }
            case 'D':
                double_val = (double *) (&args[offset]);
                offset += 8;
                break;
            case 'J':
                long_val = (uint64_t *) (&args[offset]);
                offset += 8;
                break;
        }
    }
}



// hooks




// art::mirror::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char)
void
ArtMethodInvokeHook::myArtMethodInvoke19(void *method, void *self, void *args, uint32_t args_size,
                                         void *result,
                                         char shorty) { // api19 just char
    __android_log_print(ANDROID_LOG_DEBUG,
                        "myArtMethodInvoke19",
                        "got value [-] %p-%p-%p-%d-%p-%d", method, self, args, args_size, result,
                        shorty);
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
            (void *) artMethodInvoke,
            trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke19",
                            "hooked function call original function");
        trampoline.copyOriginal();
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke19",
                            "call the function! %p", artMethodInvoke);
        ((void (*)(void *method, void *self, void *args, uint32_t args_size, void *result,
                   char shorty)) artMethodInvoke)(method,
                                                  self,
                                                  args,
                                                  args_size,
                                                  result,
                                                  shorty);
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke19",
                            "call the function done!");
        if (!trampoline.reinstall()) {
            __android_log_print(ANDROID_LOG_DEBUG,
                                "myArtMethodInvoke19",
                                "ERROR while reinstall hook");
        }
    } else {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke19",
                            "hooked function cannot call original function");
    }
}

// art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)
void
ArtMethodInvokeHook::myArtMethodInvoke22(void *method, void *self, void *args, uint32_t args_size,
                                         void *result,
                                         const char *shorty) { // api19 just char
    __android_log_print(ANDROID_LOG_DEBUG,
                        "myArtMethodInvoke22",
                        "got value [-] %p-%p-%p-%d-%p-%p", method, self, args, args_size, result,
                        shorty);
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
            (void *) artMethodInvoke,
            trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke22",
                            "hooked function call original function");
        auto callAddr = (void (*)(void *method, void *self, void *args, uint32_t args_size,
                                  void *result,
                                  char const *shorty)) trampoline.getRealCallAddr();
        if (callAddr == nullptr) {
            callAddr = (void (*)(void *method, void *self, void *args, uint32_t args_size,
                                 void *result,
                                 char const *shorty)) artMethodInvoke;
            trampoline.copyOriginal();
        }


        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke22",
                            "call the function! %p", artMethodInvoke);
        callAddr(method,
                 self,
                 args,
                 args_size,
                 result,
                 shorty);
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke22",
                            "  got result [-] %p-%p-%p-%d-%p-%p", method, self, args, args_size,
                            result,
                            shorty);
        if (getInstance().getArtMethodPrettyNameHook().availible()) {
            std::string methodName = getInstance().getArtMethodPrettyNameHook().getMethodName(
                    method);
            log() << "artMethodRuntimeMethodName Name(" << methodName << ")";
            if (_callback) {
                _callback(method, methodName);
            }
        }
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke22",
                            "call the function done!");
        if (callAddr == nullptr) {
            if (!trampoline.reinstall()) {
                __android_log_print(ANDROID_LOG_DEBUG,
                                    "myArtMethodInvoke22",
                                    "ERROR while reinstall hook");
            }
        }
    } else {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke22",
                            "hooked function cannot call original function");
    }
}


// art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)
void
ArtMethodInvokeHook::myArtMethodInvoke27(void *method, void *self, char *args, uint32_t args_size,
                                         void *result,
                                         char const *shorty) { // api19 just char
    __android_log_print(ANDROID_LOG_DEBUG,
                        "myArtMethodInvoke27",
                        "got value [-] %p-%p-%p-%d-%p-%p-%s", method, self, args, args_size, result,
                        shorty, shorty);
    //parseArgs((char *) args, args_size, shorty);
    ChickenHook::Trampoline trampoline;
    if (ChickenHook::Hooking::getInstance().getTrampolineByAddr(
            (void *) artMethodInvoke,
            trampoline)) {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke27",
                            "hooked function call original function");
        auto callAddr = (void (*)(void *method, void *self, void *args, uint32_t args_size,
                                  void *result,
                                  char const *shorty)) trampoline.getRealCallAddr();
        if (callAddr == nullptr) {
            callAddr = (void (*)(void *method, void *self, void *args, uint32_t args_size,
                                 void *result,
                                 char const *shorty)) artMethodInvoke;
            trampoline.copyOriginal();
        }

        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke27",
                            "call the function! %p", artMethodInvoke);
        callAddr(method,
                 self,
                 args,
                 args_size,
                 result,
                 shorty);
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke27",
                            "  got result [-] %p-%p-%p-%d-%p-%p", method, self, args, args_size,
                            result,
                            shorty);
        if (getInstance().getArtMethodPrettyNameHook().availible()) {
            std::string methodName = getInstance().getArtMethodPrettyNameHook().getMethodName(
                    method);
            log() << "artMethodRuntimeMethodName Name(" << methodName << ")";

            if (_callback) {
                _callback(method, methodName);
            }
        }
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke27",
                            "call the function done!");
        if (callAddr == nullptr) {
            if (!trampoline.reinstall()) {
                __android_log_print(ANDROID_LOG_DEBUG,
                                    "myArtMethodInvoke27",
                                    "ERROR while reinstall hook");
            }
        }

    } else {
        __android_log_print(ANDROID_LOG_DEBUG,
                            "myArtMethodInvoke27",
                            "hooked function cannot call original function");
    }
}

void ArtMethodInvokeHook::setCallback(void (callback)(void *, std::string)) {
    _callback = callback;
}

ArtMethodInvokeHook &ArtMethodInvokeHook::getInstance() {
    {
        static ArtMethodInvokeHook instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
}

ArtMethodPrettyNameHook ArtMethodInvokeHook::getArtMethodPrettyNameHook() {
    return artMethodPrettyName;
}
