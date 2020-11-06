#ifndef LOG_H
#define LOG_H

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

namespace Util{
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
        
    public:
        Logger(){}
        //1. std logger
        //2. file logger
        void init(enum LOGTYPE type, const std::string& path = ""){
            logtype = type;
            if(type == LOGTYPE::STD){
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                //spdlog::logger logger("multi_sink", {console_sink, file_sink});
                logger = std::make_shared<spdlog::logger>("console", console_sink);
            }else if(type == LOGTYPE::FILE){
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
    private:
        enum LOGTYPE {STD, FILE};
        std::shared_ptr<spdlog::logger> logger;
        int logtype;

    };
}

#endif