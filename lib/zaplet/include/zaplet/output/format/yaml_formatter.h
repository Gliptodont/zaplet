/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef YAML_FORMATTER_H
#define YAML_FORMATTER_H

#include "zaplet/output/formatter.h"

namespace zaplet::output
{
    class YamlFormatter : public Formatter
    {
    public:
        std::string format(const http::Response& response) const override;
        std::string getContentType() const override;
    };
} // namespace zaplet::output

#endif // YAML_FORMATTER_H
