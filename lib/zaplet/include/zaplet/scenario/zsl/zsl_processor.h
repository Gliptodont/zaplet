/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef ZSL_PROCESSOR_H
#define ZSL_PROCESSOR_H

#include "zaplet/scenario/scenario.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace zaplet::scenario::zsl
{
    class ZslProcessor
    {
    public:
        ZslProcessor() = default;

        std::optional<Scenario> parseFile(const std::filesystem::path& path);
        std::optional<Scenario> parseString(const std::string& source);

        [[nodiscard]] const std::vector<std::string>& getErrors() const;

    private:
        std::vector<std::string> m_errors;
    };
} // namespace zaplet::scenario::zsl

#endif // ZSL_PROCESSOR_H
