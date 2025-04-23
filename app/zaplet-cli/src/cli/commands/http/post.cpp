/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "cli/commands/http/post.h"

#include <zaplet/zaplet.h>

namespace zaplet::cli
{
    void PostCommand::setupOptions()
    {
        m_app->add_option("url", m_url, "URL to request")->required();
        m_app->add_option("-H,--header", m_headers, "HTTP headers (can be specified multiple times)");
        m_app->add_option("-d,--data", m_body, "Request body data");
        m_app->add_option("--content-type", m_contentType, "Content type")->default_val("application/json");
        m_app->add_option("-t,--timeout", m_timeout, "Request timeout in seconds")->default_val(30);
    }

    void PostCommand::execute()
    {
        LOG_INFO_FMT("Executing POST request to {}", m_url);

        std::map<std::string, std::string> headerMap = http::headersVectorToMap(m_headers);

        if (!m_body.empty() && headerMap.find("Content-Type") == headerMap.end())
        {
            headerMap["Content-Type"] = m_contentType;
            LOG_DEBUG_FMT("Added Content-Type: {}", m_contentType);
        }

        http::Request request;
        request.setUrl(m_url);
        request.setMethod("POST");
        request.setHeaders(headerMap);
        request.setBody(m_body);
        request.setTimeout(m_timeout);

        auto response = m_client->execute(request);

        http::printResponse(m_formatter->format(response), response.getStatusCode());
    }
} // namespace zaplet::cli
