/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/output/formatter_factory.h"

#include "zaplet/logging/logger.h"
#include "zaplet/output/format/json_formatter.h"
#include "zaplet/output/format/table_formatter.h"
#include "zaplet/output/format/yaml_formatter.h"

#include <algorithm>

namespace zaplet::output
{
    std::shared_ptr<Formatter> FormatterFactory::create(const std::string& format)
    {
        std::string lowerFormat = format;
        std::ranges::transform(lowerFormat, lowerFormat.begin(), ::tolower);

        if (lowerFormat == "table" || lowerFormat == "text")
        {
            return std::make_shared<TableFormatter>();
        }
        else if (lowerFormat == "yaml" || lowerFormat == "yml")
        {
            return std::make_shared<YamlFormatter>();
        }
        else if (lowerFormat == "json")
        {
            return std::make_shared<JsonFormatter>();
        }
        else
        {
            LOG_WARNING_FMT("Unknown format '{}', falling back to YAML", format);
            return std::make_shared<YamlFormatter>();
        }
    }
} // namespace zaplet::output