/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/zsl/zsl_processor.h"

#include "zaplet/logging/logger.h"
#include "zaplet/scenario/zsl/zsl_lexer.h"
#include "zaplet/scenario/zsl/zsl_parser.h"

#include <format>
#include <fstream>
#include <sstream>

namespace zaplet::scenario::zsl
{
    std::optional<Scenario> ZslProcessor::parseFile(const std::filesystem::path& path)
    {
        m_errors.clear();

        if (!std::filesystem::exists(path))
        {
            m_errors.push_back(std::format("File not found: {}", path.string()));
            LOG_ERROR_FMT("Scenario file not found: {}", path.string());
            return std::nullopt;
        }

        try
        {
            std::ifstream file(path);
            if (!file.is_open())
            {
                m_errors.push_back(std::format("Failed to open file: {}", path.string()));
                LOG_ERROR_FMT("Failed to open scenario file: {}", path.string());
                return std::nullopt;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            return parseString(buffer.str());
        } catch (const std::exception& e)
        {
            m_errors.push_back(std::format("Exception reading file: {}", e.what()));
            LOG_ERROR_FMT("Exception reading scenario file: {}", e.what());
            return std::nullopt;
        }
    }

    std::optional<Scenario> ZslProcessor::parseString(const std::string& source)
    {
        m_errors.clear();

        try
        {
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();

            LOG_DEBUG_FMT("Lexical analysis complete: {} tokens", tokens.size());

            Parser parser(tokens);
            std::optional<Scenario> scenario = parser.parse();

            auto errors = parser.getErrors();
            if (!errors.empty())
            {
                m_errors = errors;
                LOG_ERROR_FMT("Parsing failed with {} errors", errors.size());
                return std::nullopt;
            }

            LOG_INFO("Scenario parsed successfully");
            return scenario;
        } catch (const std::exception& e)
        {
            m_errors.push_back(std::format("Exception during parsing: {}", e.what()));
            LOG_ERROR_FMT("Exception during parsing: {}", e.what());
            return std::nullopt;
        }
    }

    const std::vector<std::string>& ZslProcessor::getErrors() const
    {
        return m_errors;
    }
} // namespace zaplet::scenario::zsl
