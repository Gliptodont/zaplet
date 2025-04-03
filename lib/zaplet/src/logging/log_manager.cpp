/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/logging/log_manager.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/tcp_sink.h>
#include <spdlog/sinks/udp_sink.h>

namespace fs = std::filesystem;

namespace zaplet::logging
{
    std::shared_ptr<LogManager> LogManager::m_instance;
    std::mutex LogManager::m_mutex;

    LogLevel string_to_level(const std::string& level)
    {
        std::string levelLower = level;
        std::transform(levelLower.begin(), levelLower.end(), levelLower.begin(), ::tolower);

        if (levelLower == "trace")
        {
            return LogLevel::Trace;
        }

        if (levelLower == "debug")
        {
            return LogLevel::Debug;
        }

        if (levelLower == "info")
        {
            return LogLevel::Info;
        }

        if (levelLower == "warning" || levelLower == "warn")
        {
            return LogLevel::Warning;
        }

        if (levelLower == "error")
        {
            return LogLevel::Error;
        }

        if (levelLower == "critical" || levelLower == "fatal")
        {
            return LogLevel::Critical;
        }

        if (levelLower == "off")
        {
            return LogLevel::Off;
        }

        return LogLevel::Info;
    }

    std::shared_ptr<LogManager> LogManager::Instance()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_instance)
        {
            m_instance = std::shared_ptr<LogManager>(new LogManager());
        }
        return m_instance;
    }

    LogManager::LogManager()
    {
        init();
    }

    void LogManager::init(LogLevel log_level, const std::string& pattern)
    {
        if (m_initialized)
        {
            spdlog::warn("Logging system already initialized");
            return;
        }

        try
        {
            spdlog::init_thread_pool(8192, 1);

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(to_spdlog_level(log_level));
            console_sink->set_pattern(pattern);

            auto logger = std::make_shared<spdlog::logger>("zaplet", console_sink);
            logger->set_level(to_spdlog_level(log_level));

            spdlog::flush_every(std::chrono::seconds(3));

            m_initialized = true;
            spdlog::debug("Logging system initialized");
        } catch (const spdlog::spdlog_ex& ex)
        {
            std::cerr << "Logging initialization failed: " << ex.what() << std::endl;
        }
    }

    bool LogManager::add_file_logger(const std::string& filename, LogLevel level)
    {
        try
        {
            fs::path file_path(filename);
            if (!file_path.parent_path().empty())
            {
                fs::create_directories(file_path.parent_path());
            }

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
            file_sink->set_level(to_spdlog_level(level));

            spdlog::default_logger()->sinks().push_back(file_sink);

            spdlog::info("Added file logger: {}", filename);
            return true;
        } catch (const spdlog::spdlog_ex& ex)
        {
            spdlog::error("Failed to add file logger: {}", ex.what());
            return false;
        }
    }

    bool LogManager::add_remote_logger(const std::string& host, int port, const std::string& protocol, LogLevel level)
    {
        try
        {
            std::shared_ptr<spdlog::sinks::sink> network_sink;

            if (protocol == "tcp")
            {
                spdlog::sinks::tcp_sink_config config(host, port);
                network_sink = std::make_shared<spdlog::sinks::tcp_sink_mt>(config);
            }
            else if (protocol == "udp")
            {
                spdlog::sinks::udp_sink_config config(host, static_cast<uint16_t>(port));
                network_sink = std::make_shared<spdlog::sinks::udp_sink_mt>(config);
            }
            else
            {
                spdlog::error("Unknown protocol: {}, expected 'tcp' or 'udp'", protocol);
                return false;
            }

            network_sink->set_level(to_spdlog_level(level));

            spdlog::default_logger()->sinks().push_back(network_sink);

            spdlog::info("Added remote logger: {}:{} ({})", host, port, protocol);
            return true;
        } catch (const spdlog::spdlog_ex& ex)
        {
            spdlog::error("Failed to add remote logger: {}", ex.what());
            return false;
        }
    }

    void LogManager::set_level(LogLevel level)
    {
        spdlog::set_level(to_spdlog_level(level));
    }

    void LogManager::flush(LogLevel level)
    {
        spdlog::flush_on(to_spdlog_level(level));
    }

    spdlog::level::level_enum LogManager::to_spdlog_level(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace:
            return spdlog::level::trace;
        case LogLevel::Debug:
            return spdlog::level::debug;
        case LogLevel::Info:
            return spdlog::level::info;
        case LogLevel::Warning:
            return spdlog::level::warn;
        case LogLevel::Error:
            return spdlog::level::err;
        case LogLevel::Critical:
            return spdlog::level::critical;
        case LogLevel::Off:
            return spdlog::level::off;
        default:
            return spdlog::level::info;
        }
    }

} // namespace zaplet::logging