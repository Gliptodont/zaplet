/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/output/format/table_formatter.h"

#include <format>
#include <iomanip>
#include <istream>
#include <sstream>

namespace zaplet::output
{
    std::string TableFormatter::format(const http::Response& response) const
    {
        std::ostringstream oss;

        oss << std::format("╔══════════════════════════════════════════════════════╗\n");
        oss << std::format("║ Status: {:3d} {:<42} ║\n", response.getStatusCode(), response.isSuccess() ? "Success" : "Failed");
        oss << std::format("║ Latency: {:<44} ms ║\n", response.getLatency().count());

        if (response.hasError())
        {
            oss << std::format("╠══════════════════════════════════════════════════════╣\n");
            oss << std::format("║ Error: {:<45} ║\n", response.getError().value());
        }

        oss << std::format("╠══════════════════════════════════════════════════════╣\n");
        oss << std::format("║ Headers                                              ║\n");
        oss << std::format("╠══════════════════════════╦═══════════════════════════╣\n");

        for (const auto& [name, value] : response.getHeaders())
        {
            std::string truncatedName = name;
            std::string truncatedValue = value;

            if (truncatedName.length() > 24)
            {
                truncatedName = truncatedName.substr(0, 21) + "...";
            }

            if (truncatedValue.length() > 25)
            {
                truncatedValue = truncatedValue.substr(0, 22) + "...";
            }

            oss << std::format("║ {:<24} ║ {:<23} ║\n", truncatedName, truncatedValue);
        }

        oss << std::format("╠══════════════════════════╩═══════════════════════════╣\n");
        oss << std::format("║ Body                                                 ║\n");
        oss << std::format("╠══════════════════════════════════════════════════════╣\n");

        std::string bodyText = response.getBody();
        const size_t maxLineWidth = 50;

        if (bodyText.length() > 500)
        {
            bodyText = bodyText.substr(0, 500) + "\n...(truncated)...";
        }

        std::istringstream bodyStream(bodyText);
        std::string line;

        while (std::getline(bodyStream, line))
        {
            size_t pos = 0;
            while (pos < line.length())
            {
                size_t chunk = std::min(maxLineWidth, line.length() - pos);
                oss << std::format("║ {:<50} ║\n", line.substr(pos, chunk));
                pos += chunk;
            }
        }

        oss << std::format("╚══════════════════════════════════════════════════════╝\n");

        return oss.str();
    }

    std::string TableFormatter::getContentType() const
    {
        return "text/plain";
    }
} // namespace zaplet::output
