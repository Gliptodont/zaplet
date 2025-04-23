/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef HTTPUTILS_H
#define HTTPUTILS_H

#include "zaplet/logging/logger.h"

#include <map>
#include <string>
#include <vector>

namespace zaplet::http
{
    inline std::map<std::string, std::string> headersVectorToMap(const std::vector<std::string>& headersVector)
    {
        std::map<std::string, std::string> headerMap;
        for (const auto& header : headersVector)
        {
            auto colonPos = header.find(':');
            if (colonPos != std::string::npos)
            {
                std::string key = header.substr(0, colonPos);
                std::string value = header.substr(colonPos + 1);

                value.erase(0, value.find_first_not_of(" \t"));

                headerMap[key] = value;
                LOG_DEBUG_FMT("Header: {}={}", key, value);
            }
            else
            {
                LOG_WARNING_FMT("Invalid header format: {}", header);
            }
        }

        return headerMap;
    }
}

#endif // HTTPUTILS_H
