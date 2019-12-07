
#include <iostream>
#include <sstream>
#include <android/log.h>

struct log
{
    log() {
        uncaught = std::uncaught_exceptions();
    }

    ~log() {
        if (uncaught >= std::uncaught_exceptions()) {
            __android_log_print(ANDROID_LOG_DEBUG,"AndroidChickenTime","%s",stream.str().c_str());
        }
    }

    std::stringstream stream;
    int uncaught;
};

template <typename T>
log& operator<<(log& record, T&& t) {
    record.stream << std::forward<T>(t);
    return record;
}

template <typename T>
log& operator<<(log&& record, T&& t) {
    return record << std::forward<T>(t);
}