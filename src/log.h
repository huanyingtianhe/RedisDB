#ifndef LOG_H
#define LOG_H

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

template <typename T>
class Singleton {
    static T& getInstance() {
        static T instance;
        return instance;
    }
    public:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton& ) = delete;
    Singleton& operator=(const Singleton&) = delete;
};


class Logger {
    std::shared_ptr<spdlog::logger> logger;
    int logtype;
public:
    void Logger(){}
    //1. std logger
    //2. fiel logger
    void init(int type, const std::string& path = ""){
        logtype = type;
        if(type == 1){
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            //spdlog::logger logger("multi_sink", {console_sink, file_sink});
            logger = std::make_shared<spdlog::logger>("console", console_sink);
        }else if(type == 2){
            logger = spdlog::daily_logger_mt("daily_logger", path, 0, 0);
        }
    }
    //debug, info, warn, error output
    void debug(const std::string& s){
        logger->debug(s);
    }

    void info(const std::string& s){
        logger->info(s);
    }

    void warn(const std::string& s){
        logger->warn(s);
    }

    void error(const std::string& s){
        logger->error(s);
    }


};

#endif