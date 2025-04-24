/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

#include "zaplet/ini/INIreader.h"
#include "zaplet/logging/utils.h"

#include <spdlog/spdlog.h>

#include <string>

namespace zaplet::logging
{
    struct NetworkConfig
    {
        std::string host;
        uint16_t port;
        bool enabled = false;
    };

    struct Config
    {
        std::string loggerName = "zaplet";
        LogLevel level = LogLevel::Info;
        std::string pattern = "%^[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v%$";
        bool async = false;
        size_t asyncQueueSize = 8192;
        size_t asyncThreadCount = 1;

        // file logging
        bool fileLoggingEnabled = true;
        std::string filePath = "logs/zaplet.log";
        bool rotatingFile = false;
        size_t maxFilesSize = 10 * 1024 * 1024;
        size_t maxFiles = 10;
        bool dailyFile = false;
        int rotationHour = 0;
        int rotationMinute = 0;

        // console logging
        bool consoleLoggingEnabled = true;

        // network logging
        NetworkConfig tcp;
        NetworkConfig udp;
    };

    inline Config loadConfigFromIni(const std::string& configPath)
    {
        INIReader reader(configPath);

        if (reader.ParseError() != 0)
        {
            throw std::runtime_error("Failed to parse ini file: " + configPath);
        }

        Config config;

        // general settings
        config.loggerName = reader.Get("general", "name", "application");
        config.level = stringToLogLevel(reader.Get("general", "level", "info"));
        config.pattern = reader.Get("general", "pattern", "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] %v");
        config.async = reader.GetBoolean("general", "async", false);
        config.asyncQueueSize = reader.GetInteger("general", "async_queue_size", 8192);
        config.asyncThreadCount = reader.GetInteger("general", "async_thread_count", 1);

        // console settings
        config.consoleLoggingEnabled = reader.GetBoolean("console", "enabled", true);

        // file settings
        config.fileLoggingEnabled = reader.GetBoolean("file", "enabled", true);
        config.filePath = reader.Get("file", "path", "logs/app.log");
        config.rotatingFile = reader.GetBoolean("file", "rotating", false);
        config.maxFilesSize = reader.GetInteger("file", "max_size", 10 * 1024 * 1024);
        config.maxFiles = reader.GetInteger("file", "max_files", 10);
        config.dailyFile = reader.GetBoolean("file", "daily", false);
        config.rotationHour = reader.GetInteger("file", "rotation_hour", 0);
        config.rotationMinute = reader.GetInteger("file", "rotation_minute", 0);

        // network settings - tcp
        config.tcp.enabled = reader.GetBoolean("tcp", "enabled", false);
        config.tcp.host = reader.Get("tcp", "host", "127.0.0.1");
        config.tcp.port = static_cast<uint16_t>(reader.GetInteger("tcp", "port", 9000));

        // network setting - udp
        config.udp.enabled = reader.GetBoolean("udp", "enabled", false);
        config.udp.host = reader.Get("udp", "host", "127.0.0.1");
        config.udp.port = static_cast<uint16_t>(reader.GetInteger("udp", "port", 9001));

        return config;
    }
} // namespace zaplet::logging

#endif // LOG_CONFIG_H
