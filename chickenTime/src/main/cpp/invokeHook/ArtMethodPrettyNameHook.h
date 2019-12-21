//
// Created by Sascha Roth on 16.12.19.
//

#pragma once


#include <jni.h>

class ArtMethodPrettyNameHook {
public:
    bool installHooks(JavaVM *vm, void* libart);

    std::string getMethodName(void *artMethod);

    bool availible();

private:
    void *artMethodPrettyMethod = nullptr;

};
