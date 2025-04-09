/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef JSON_FORMATTER_H
#define JSON_FORMATTER_H

#include "zaplet/output/formatter.h"

namespace zaplet::output
{
    class JsonFormatter : public Formmater
    {
    public:
        std::string format(const http::Response& response) const override;
        std::string getContentType() const override;
    };
}

#endif // JSON_FORMATTER_H
