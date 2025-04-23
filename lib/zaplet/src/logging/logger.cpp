/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/logging/logger.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/tcp_sink.h>
#include <spdlog/sinks/udp_sink.h>

#include <filesystem>
#include <chrono>

namespace zaplet::logging
{
    namespace fs = std::filesystem;

    Logger& Logger::getInstance()
    {
        static Logger instance;
        return instance;
    }

    void Logger::initialize()
    {
        getInstance().init(Config());
    }

    void Logger::initialize(const Config& config)
    {
        getInstance().init(config);
    }

    void Logger::initializeFromIni(const std::string& configPath)
    {
        if (!fs::exists(configPath))
        {
            getInstance().init(Config());
            return;
        }

        Config config = loadConfigFromIni(configPath);
        getInstance().init(config);
    }

    void Logger::reconfigure(const Config& config)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config = config;
        reinitialize();
    }

    bool Logger::reconfigureFromIni(const std::string& configPath)
    {
        try
        {
            Config newConfig = loadConfigFromIni(configPath);
            reconfigure(newConfig);
            return true;

        } catch (const std::exception& e)
        {
            if (m_logger)
            {
                m_logger->error("Failed to reconfigure logger from ini file: {}", e.what());
            }
            return false;
        }
    }

    void Logger::trace(const std::string& msg) const
    {
        m_logger->trace(msg);
    }

    void Logger::debug(const std::string& msg) const
    {
        m_logger->debug(msg);
    }

    void Logger::info(const std::string& msg) const
    {
        m_logger->info(msg);
    }

    void Logger::warning(const std::string& msg) const
    {
        m_logger->warn(msg);
    }

    void Logger::error(const std::string& msg) const
    {
        m_logger->error(msg);
    }

    void Logger::fatal(const std::string& msg) const
    {
        m_logger->critical(msg);
    }

    Logger::Logger()
    {
        init(Config());
    }

    void Logger::init(const Config& config)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config = config;
        reinitialize();
    }

    void Logger::reinitialize()
    {
        spdlog::drop(m_config.loggerName);

        if (m_config.async)
        {
            spdlog::shutdown();
            spdlog::init_thread_pool(m_config.asyncQueueSize, m_config.asyncThreadCount);
        }

        std::vector<spdlog::sink_ptr> sinks;

        // console sink
        if (m_config.consoleLoggingEnabled)
        {
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink->set_pattern(m_config.pattern);
            sinks.push_back(consoleSink);
        }

        // file sink
        if (m_config.fileLoggingEnabled)
        {
            fs::path logFilePath(m_config.filePath);

            if (!logFilePath.parent_path().empty())
            {
                fs::create_directories(logFilePath.parent_path());
            }

            spdlog::sink_ptr fileSink;

            if (m_config.rotatingFile)
            {
                fileSink =
                    std::make_shared<spdlog::sinks::rotating_file_sink_mt>(m_config.filePath, m_config.maxFilesSize, m_config.maxFiles);
            }
            else if (m_config.dailyFile)
            {
                fileSink =
                    std::make_shared<spdlog::sinks::daily_file_sink_mt>(m_config.filePath, m_config.rotationHour, m_config.rotationMinute);
            }
            else
            {
                fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_config.filePath);
            }

            fileSink->set_pattern(m_config.pattern);
            sinks.push_back(fileSink);
        }

        if (m_config.tcp.enabled)
        {
            spdlog::sinks::tcp_sink_config tcpConfig(m_config.tcp.host, m_config.tcp.port);
            auto tcpSink = std::make_shared<spdlog::sinks::tcp_sink_mt>(tcpConfig);
            tcpSink->set_pattern(m_config.pattern);
            sinks.push_back(tcpSink);
        }

        if (m_config.udp.enabled)
        {
            spdlog::sinks::udp_sink_config udpConfig(m_config.udp.host, m_config.udp.port);
            auto udpSink = std::make_shared<spdlog::sinks::udp_sink_mt>(udpConfig);
            udpSink->set_pattern(m_config.pattern);
            sinks.push_back(udpSink);
        }

        if (m_config.async)
        {
            m_logger = std::make_shared<spdlog::async_logger>(
                m_config.loggerName, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
            m_logger = std::make_shared<spdlog::logger>(m_config.loggerName, sinks.begin(), sinks.end());
        }

        spdlog::flush_every(std::chrono::seconds(3));
        m_logger->set_level(convertLogLevel(m_config.level));

        spdlog::register_logger(m_logger);

        if (m_config.loggerName == "zaplet")
        {
            spdlog::set_default_logger(m_logger);
        }
    }

    void Logger::flush() const
    {
        spdlog::flush_on(convertLogLevel(LogLevel::Trace));
        spdlog::flush_on(convertLogLevel(LogLevel::Debug));
        spdlog::flush_on(convertLogLevel(LogLevel::Info));
        spdlog::flush_on(convertLogLevel(LogLevel::Warning));
        spdlog::flush_on(convertLogLevel(LogLevel::Error));
        spdlog::flush_on(convertLogLevel(LogLevel::Fatal));
    }
} // namespace zaplet::logging