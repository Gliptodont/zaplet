/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/output/formatter_factory.h"

#include "zaplet/logging/logger.h"

#include <algorithm>

namespace zaplet::output
{
    std::shared_ptr<Formmater> FormatterFactory::create(const std::string& format)
    {
        std::string lowerFormat = format;
        std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);

        if (lowerFormat == "table" || lowerFormat == "text")
        {

        }
        else if (lowerFormat == "yaml" || lowerFormat == "yml")
        {

        }
        else if (lowerFormat == "json")
        {

        }
        else
        {
            LOG_WARNING_FMT("Unknown format '{}', falling back to JSON", format);
        }
    }
} // namespace zaplet::output