//
// Created by sascharoth on 07.12.19.
//

#pragma once


#include "ArtMethodPrettyNameHook.h"

class ArtMethodInvokeHook {
public:
    /**
     *
     * @param libart the dlopen handle of libart
     */
    void installArtHooks(JavaVM *vm, void *libart);

    void setCallback(void (*)(void *method, std::string name));

    static ArtMethodInvokeHook &getInstance();

    ArtMethodPrettyNameHook getArtMethodPrettyNameHook();

private:

    ArtMethodPrettyNameHook artMethodPrettyName;

    static void (*_callback)(void *, std::string name);

    static void
    myArtMethodInvoke27(void *method, void *self, char *args, uint32_t args_size, void *result,
                        char const *shorty);

    static void
    myArtMethodInvoke22(void *method, void *self, void *args, uint32_t args_size, void *result,
                        const char *shorty);

    static void
    myArtMethodInvoke19(void *method, void *self, void *args, uint32_t args_size, void *result,
                        char shorty);

};