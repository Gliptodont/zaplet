/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/http/client.h"

#include "zaplet/logging/logger.h"

#include <httplib.h>

#include <chrono>
#include <format>
#include <regex>

namespace zaplet::http
{
    Response Client::execute(const Request& request)
    {
        Response response;

        try
        {
            std::string scheme;
            std::string host;
            std::string path;
            int port;

            if (!parseUrl(request.getUrl(), scheme, host, port, path))
            {
                response.setError("Invalid URL: " + request.getUrl());
                return response;
            }

            auto client = createClient(request.getUrl());
            if (!client)
            {
                response.setError("Failed to create HTTP client");
                return response;
            }

            client->setConnectionTimeout(std::chrono::seconds(request.getTimeout()));

            httplib::Headers headers;
            for (const auto& [name, value] : request.getHeaders())
            {
                headers.emplace(name, value);
            }

            auto startTime = std::chrono::steady_clock::now();

            httplib::Result result;

            if (request.getMethod() == "GET")
            {
                result = client->get(path, headers);
            }
            else if (request.getMethod() == "POST")
            {
                if (request.getBody())
                {
                    result = client->post(path, headers, request.getBody().value());
                }
                else
                {
                    result = client->post(path, headers);
                }
            }
            else if (request.getMethod() == "PUT")
            {
                if (request.getBody())
                {
                    result = client->put(path, headers, request.getBody().value());
                }
                else
                {
                    result = client->put(path, headers);
                }
            }
            else if (request.getMethod() == "DELETE")
            {
                result = client->del(path, headers);
            }
            else if (request.getMethod() == "HEAD")
            {
                result = client->head(path, headers);
            }
            else if (request.getMethod() == "OPTIONS")
            {
                result = client->options(path, headers);
            }
            else if (request.getMethod() == "PATCH")
            {
                if (request.getBody())
                {
                    result = client->patch(path, headers, request.getBody().value());
                }
                else
                {
                    result = client->patch(path, headers);
                }
            }
            else
            {
                response.setError(std::format("Unsupported HTTP method: {}", request.getMethod()));
                return response;
            }

            auto endTime = std::chrono::steady_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

            response.setLatency(latency);

            if (result)
            {
                response.setStatusCode(result->status);

                std::map<std::string, std::string> responseHeaders;
                for (const auto& [name, value] : result->headers)
                {
                    responseHeaders[name] = value;
                }
                response.setHeaders(responseHeaders);

                response.setBody(result->body);
            }
            else
            {
                auto err = result.error();
                response.setError(httplib::to_string(err));
                LOG_ERROR_FMT("HTTP error: {}", httplib::to_string(err));
            }
        } catch (const std::exception& e)
        {
            response.setError(std::format("Exception during HTTP request: {}", e.what()));
            LOG_ERROR_FMT("Exception during HTTP request: {}", e.what());
        }

        return response;
    }

    std::unique_ptr<IClientWrapper> Client::createClient(const std::string& url)
    {
        std::string scheme;
        std::string host;
        std::string path;
        int port;

        if (!parseUrl(url, scheme, host, port, path))
        {
            LOG_ERROR_FMT("Failed to parse URL: {}", url);
            return nullptr;
        }

        if (scheme == "http")
        {
            return std::make_unique<ClientWrapper>(host, port);
        }
        else if (scheme == "https")
        {
            return std::make_unique<SSLClientWrapper>(host, port);
        }
        else
        {
            LOG_ERROR_FMT("Unsupported scheme: {}", scheme);
            return nullptr;
        }
    }

    bool Client::parseUrl(const std::string& url, std::string& scheme, std::string& host, int& port, std::string& path)
    {
        std::regex urlRegex(R"(^(http|https)://([^/:]+)(?::(\d+))?(/.*)?$)");
        std::smatch matches;

        if (std::regex_match(url, matches, urlRegex))
        {
            scheme = matches[1].str();
            host = matches[2].str();

            if (matches[3].length() > 0)
            {
                port = std::stoi(matches[3].str());
            }
            else
            {
                port = (scheme == "https") ? 443 : 80;
            }

            path = matches[4].matched ? matches[4].str() : "/";

            return true;
        }

        return false;
    }
} // namespace zaplet::http
