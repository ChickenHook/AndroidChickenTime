//
// Created by Sascha Roth on 16.12.19.
//

#include <logging/logging.h>
#include <dlfcn.h>
#include "ArtMethodPrettyNameHook.h"

bool ArtMethodPrettyNameHook::installHooks(JavaVM *vm, void *libart) {

    // name of method
    artMethodPrettyMethod = dlsym(libart, "_ZN3art9ArtMethod12PrettyMethodEb");
    if (artMethodPrettyMethod == nullptr) {
        artMethodPrettyMethod = dlsym(libart, "_ZN3art9ArtMethod12PrettyMethodEPS0_b");
        if (artMethodPrettyMethod == nullptr) {
            artMethodPrettyMethod = dlsym(libart, "_ZN3art12PrettyMethodEPNS_6mirror9ArtMethodEb");

        }
        log() << "installHooks " << "!! Warning !! could not retrieve getNameAsString";
    }
    return false;
}

std::string ArtMethodPrettyNameHook::getMethodName(void *artMethod) {
    return ((std::string(*)(void *, bool)) artMethodPrettyMethod)(artMethod,
                                                                  true);
}

bool ArtMethodPrettyNameHook::availible() {
    return artMethodPrettyMethod != nullptr;
}
