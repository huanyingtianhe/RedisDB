#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

namespace Util
{
    template <typename T>
    class Singleton
    {
        static T &getInstance()
        {
            static T instance;
            return instance;
        }

    public:
        Singleton() = delete;
        ~Singleton() = delete;
        Singleton(const Singleton &) = delete;
        Singleton &operator=(const Singleton &) = delete;
    };

    class Logger
    {

    public:
        Logger() {}
        //1. std logger
        //2. file logger
        void init(enum LOGTYPE type, const std::string &path = "")
        {
            logtype = type;
            if (type == LOGTYPE::STD)
            {
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                //spdlog::logger logger("multi_sink", {console_sink, file_sink});
                logger = std::make_shared<spdlog::logger>("console", console_sink);
            }
            else if (type == LOGTYPE::FILE)
            {
                logger = spdlog::daily_logger_mt("daily_logger", path, 0, 0);
            }
        }
        //debug, info, warn, error output
        void debug(const std::string &s)
        {
            logger->debug(s);
        }

        void info(const std::string &s)
        {
            logger->info(s);
        }

        void warn(const std::string &s)
        {
            logger->warn(s);
        }

        void error(const std::string &s)
        {
            logger->error(s);
        }

    private:
        enum LOGTYPE
        {
            STD,
            FILE
        };
        std::shared_ptr<spdlog::logger> logger;
        int logtype;
    };

    class noncopyable
    {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;

        noncopyable(noncopyable const &) = delete;
        noncopyable &operator=(noncopyable const &) = delete;
    };

    //type traits
    template <class T>
    struct isPair :  std::false_type {};

    template <class T1, class T2>
    struct isPair<std::pair<T1, T2>> : std::true_type {};

    class BitsOp{
        public:
            /* Function to reverse bits. Algorithm from:
            * http://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel 
            */
            static unsigned long rev(unsigned long v) {
                unsigned long s = 8 * sizeof(v); // bit size; must be power of 2
                unsigned long mask = ~0;
                while ((s >>= 1) > 0) {
                    mask ^= (mask << s);
                    v = ((v >> s) & mask) | ((v << s) & ~mask);
                }
                return v;
            }
    };

}; // namespace Util

#endif