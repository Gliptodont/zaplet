/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "zaplet/logging/log_config.h"

#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <string>
#include <format>

namespace zaplet::logging
{
    class Logger
    {
    public:
        static Logger& getInstance();

        static void initialize();
        static void initialize(const Config& config);
        static void initializeFromIni(const std::string& configPath);

        [[nodiscard]] std::shared_ptr<spdlog::logger> get() const
        {
            return m_logger;
        }

        void reconfigure(const Config& config);
        bool reconfigureFromIni(const std::string& configPath);

        void trace(const std::string& msg) const;
        void debug(const std::string& msg) const;
        void info(const std::string& msg) const;
        void warning(const std::string& msg) const;
        void error(const std::string& msg) const;
        void fatal(const std::string& msg) const;

        void flush() const;
    private:
        Logger();

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        void init(const Config& config);
        void reinitialize();

        std::shared_ptr<spdlog::logger> m_logger;
        Config m_config;
        std::mutex m_mutex;
    };

#define LOG_INITIALIZE() zaplet::logging::Logger::initialize()
#define LOG_INITIALIZE_WITH_CONFIG(config) ::zaplet::logging::Logger::initialize(config)
#define LOG_INITIALIZE_FROM_INI(path) ::zaplet::logging::Logger::initializeFromIni(path)

#define LOG_TRACE(msg) zaplet::logging::Logger::getInstance().trace(msg)
#define LOG_DEBUG(msg) zaplet::logging::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) zaplet::logging::Logger::getInstance().info(msg)
#define LOG_WARNING(msg) zaplet::logging::Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) zaplet::logging::Logger::getInstance().error(msg)
#define LOG_FATAL(msg) zaplet::logging::Logger::getInstance().fatal(msg)

#define LOG_TRACE_FMT(msg, ...) zaplet::logging::Logger::getInstance().trace(std::format(msg, __VA_ARGS__))
#define LOG_DEBUG_FMT(msg, ...) zaplet::logging::Logger::getInstance().debug(std::format(msg, __VA_ARGS__))
#define LOG_INFO_FMT(msg, ...) zaplet::logging::Logger::getInstance().info(std::format(msg, __VA_ARGS__))
#define LOG_WARNING_FMT(msg, ...) zaplet::logging::Logger::getInstance().warning(std::format(msg, __VA_ARGS__))
#define LOG_ERROR_FMT(msg, ...) zaplet::logging::Logger::getInstance().error(std::format(msg, __VA_ARGS__))
#define LOG_FATAL_FMT(msg, ...) zaplet::logging::Logger::getInstance().fatal(std::format(msg, __VA_ARGS__))
} // namespace zaplet::logging

#endif // LOGGER_H
