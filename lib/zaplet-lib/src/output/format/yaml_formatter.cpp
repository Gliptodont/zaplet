/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/output/format/yaml_formatter.h"

#include <nlohmann/json.hpp>

#include <sstream>

namespace zaplet::output
{
    static std::string jsonToYaml(const nlohmann::json& json, int indentLevel = 0)
    {
        std::string indent(indentLevel * 2, ' ');
        std::ostringstream yaml;

        if (json.is_object())
        {
            if (indentLevel > 0)
            {
                yaml << "\n";
            }

            for (auto it = json.begin(); it != json.end(); ++it)
            {
                yaml << indent << it.key() << ": ";
                yaml << jsonToYaml(it.value(), indentLevel + 1);

                if (indentLevel > 0 || std::next(it) != json.end())
                {
                    yaml << "\n";
                }
            }
        }
        else if (json.is_array())
        {
            if (indentLevel > 0)
            {
                yaml << "\n";
            }

            for (auto it = json.begin(); it != json.end(); ++it)
            {
                yaml << indent << "- " << jsonToYaml(*it, indentLevel + 1);

                if (indentLevel > 0 || std::next(it) != json.end())
                {
                    yaml << "\n";
                }
            }
        }
        else if (json.is_string())
        {
            std::string s = json.get<std::string>();

            if (s.empty() || s.find_first_of(":#{}[]&*!|>'\"%@`") != std::string::npos)
            {
                yaml << "\"" << s << "\"";
            }
            else
            {
                yaml << s;
            }
        }
        else
        {
            yaml << json.dump();
        }

        return yaml.str();
    }

    std::string YamlFormatter::format(const http::Response& response) const
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

        return std::format("---\n{}", jsonToYaml(jsonResponse));
    }

    std::string YamlFormatter::getContentType() const
    {
        return "application/yaml";
    }
} // namespace zaplet::output
