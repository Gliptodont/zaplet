/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/output/format/json_formatter.h"

#include <nlohmann/json.hpp>

namespace zaplet::output
{
    std::string JsonFormatter::format(const http::Response& response) const
    {
        nlohmann::json jsonResponse;

        jsonResponse["status_code"] = response.getStatusCode();
        jsonResponse["success"] = response.isSuccess();
        jsonResponse["latency_ms"] = response.getLatency().count();

        nlohmann::json headers;
        for (const auto& [name, value] : response.getHeaders())
        {
            headers[name] = value;
        }
        jsonResponse["headers"] = headers;

        if (response.getHeaders().count("Content-Type") > 0 && response.getHeaders().at("Content-Type").find("application/json") != std::string::npos)
        {
            try
            {
                jsonResponse["body"] = nlohmann::json::parse(response.getBody());
            }
            catch (const nlohmann::json::exception&)
            {
                jsonResponse["body"] = response.getBody();
            }
        }
        else
        {
            jsonResponse["body"] = response.getBody();
        }

        if (response.hasError())
        {
            jsonResponse["error"] = response.getError().value();
        }

        return jsonResponse.dump(2);
    }

    std::string JsonFormatter::getContentType() const
    {
        return "application/json";
    }
} // namespace zaplet::output
