//
// Created by Sascha Roth on 07.12.19.
//

#pragma once

#include <string>
#include <jni.h>
#include <dlfcn.h>

class ArtPathFinder {
public:
    static std::string getArtPath(JNIEnv *env);

private:


    static std::string gdladdrVariant(JNIEnv *env);
};