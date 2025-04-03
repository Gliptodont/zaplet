/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <memory>
#include <mutex>
#include <string>

#include <spdlog/spdlog.h>

namespace zaplet::logging
{
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical,
        Off
    };

    LogLevel string_to_level(const std::string& level);

    class LogManager
    {
    public:
        static std::shared_ptr<LogManager> Instance();

        void init(LogLevel log_level = LogLevel::Info, const std::string& pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        bool add_file_logger(const std::string& filename, LogLevel level = LogLevel::Info);
        bool add_remote_logger(const std::string& host, int port, const std::string& protocol = "tcp", LogLevel level = LogLevel::Info);

        void set_level(LogLevel level);

        void flush(LogLevel level);

    private:
        LogManager();

        spdlog::level::level_enum to_spdlog_level(LogLevel level);

        static std::shared_ptr<LogManager> m_instance;
        static std::mutex m_mutex;

        bool m_initialized = false;
    };
} // namespace zaplet::logging

#endif // LOG_MANAGER_H
