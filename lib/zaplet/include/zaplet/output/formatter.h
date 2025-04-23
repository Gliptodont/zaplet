/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include "zaplet/http/response.h"

#include <string>

namespace zaplet::output
{
    class Formatter
    {
    public:
        virtual ~Formatter() = default;

        virtual std::string format(const http::Response& response) const = 0;
        virtual std::string getContentType() const = 0;
    };
} // namespace zaplet::output

#endif // FORMATTER_H
