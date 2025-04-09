/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef FORMATTER_FACTORY_H
#define FORMATTER_FACTORY_H

#include "zaplet/output/formatter.h"

#include <memory>
#include <string>

namespace zaplet::output
{
    class FormatterFactory
    {
    public:
        static std::shared_ptr<Formmater> create(const std::string& format);
    };
}

#endif // FORMATTER_FACTORY_H
