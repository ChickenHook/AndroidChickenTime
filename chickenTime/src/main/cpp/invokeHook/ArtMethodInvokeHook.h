//
// Created by sascharoth on 07.12.19.
//

#pragma once


class ArtMethodInvokeHook {
public:
    /**
     *
     * @param libart the dlopen handle of libart
     */
    static void installArtHooks(JavaVM *vm, void *libart);

    static void setCallback(void (*)(void *method));

private:

    static void (*_callback)(void *);

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