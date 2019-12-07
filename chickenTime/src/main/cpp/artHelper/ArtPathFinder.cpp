//
// Created by Sascha Roth on 07.12.19.
//

#include "ArtPathFinder.h"
#include <string>
#include <jni.h>
#include <logging/logging.h>


std::string ArtPathFinder::gdladdrVariant(JNIEnv *env) {
    Dl_info __info;
    int infoAvailable = 0;
    void *addr = (void *) env->functions->FatalError;
    if (addr == nullptr) {
        // alternatives?
        log() << "ArtPathFinder [+] getArtPath [-] " << "Cannot retrieve art function :(";
        return "";
    }

    infoAvailable = dladdr(addr, &__info);

    if (infoAvailable != 0) {
        return __info.dli_fname;
    } else {
        log() << "ArtPathFinder [+] getArtPath [-] " << "Cannot retrieve dlinfo :(";
        return "";
    }
}

std::string ArtPathFinder::getArtPath(JNIEnv *env) {
    std::string artPath;

    artPath = gdladdrVariant(env);

    if (artPath.length() == 0) {
        log() << "ArtPathFinder [+] getArtPath [-] " << "Cannot retrieve art function using dladdr";
        return "";
    } else {
        log() << "ArtPathFinder [+] getArtPath [+] artPath <" << artPath << ">";

    }

    return artPath;
}