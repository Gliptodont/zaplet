/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef TABLE_FORMATTER_H
#define TABLE_FORMATTER_H

#include "zaplet/output/formatter.h"

namespace zaplet::output
{
    class TableFormatter : public Formatter
    {
    public:
        std::string format(const http::Response& response) const override;
        std::string getContentType() const override;
    };
}

#endif // TABLE_FORMATTER_H
