/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef UTILS_H
#define UTILS_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <string>

namespace zaplet::logging
{
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        Off
    };

    inline spdlog::level::level_enum convertLogLevel(LogLevel level)
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
        case LogLevel::Fatal:
            return spdlog::level::critical;
        case LogLevel::Off:
            return spdlog::level::off;
        default:
            return spdlog::level::info;
        }
    }

    inline LogLevel stringToLogLevel(const std::string& levelStr)
    {
        std::string lowerLevel;
        lowerLevel.resize(levelStr.size());
        std::transform(levelStr.begin(), levelStr.end(), lowerLevel.begin(), ::tolower);

        if (lowerLevel == "trace")
        {
            return LogLevel::Trace;
        }

        if (lowerLevel == "debug")
        {
            return LogLevel::Debug;
        }

        if (lowerLevel == "info")
        {
            return LogLevel::Info;
        }

        if (lowerLevel == "warning")
        {
            return LogLevel::Warning;
        }

        if (lowerLevel == "error")
        {
            return LogLevel::Error;
        }

        if (lowerLevel == "fatal")
        {
            return LogLevel::Fatal;
        }

        if (lowerLevel == "off")
        {
            return LogLevel::Off;
        }

        return LogLevel::Info;
    }
} // namespace zaplet::logging

#endif // UTILS_H
