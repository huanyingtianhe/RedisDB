#ifndef LOG_H
#define LOG_H

#include <spdlog/sinks/stdout_color_sinks.h>

template <typename T>
class Singleton {
    static T& getInstance() {
        static T instance;
        return instance;
    }
    public:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton& ) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

class Logger: public Singleton<Logger> {
    
};

#endif