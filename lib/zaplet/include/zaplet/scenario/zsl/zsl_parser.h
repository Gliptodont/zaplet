/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef ZSL_PARSER_H
#define ZSL_PARSER_H

#include "zaplet/scenario/scenario.h"
#include "zaplet/scenario/step.h"
#include "zaplet/scenario/zsl/zsl_lexer.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace zaplet::scenario::zsl
{
    class Parser
    {
    public:
        explicit Parser(std::vector<Token> tokens);

        std::optional<Scenario> parse();

        const std::vector<std::string>& getErrors() const;

        void dumpToken(const Token& token, const std::string& context);
        void dumpCurrentToken(const std::string& context);

    private:
        std::vector<Token> m_tokens;
        int m_current = 0;
        Scenario m_scenario;
        std::vector<std::string> m_errors;

        bool match(TokenType type);
        bool match(const std::initializer_list<TokenType>& types);
        bool check(TokenType type) const;
        bool check(const std::initializer_list<TokenType>& types) const;

        const Token& previous() const;
        const Token& advance();
        const Token& peek() const;

        bool isAtEnd() const;
        void error(const std::string& message);

        void parseScenario();
        void parseMetadata(TokenType metadataType);
        Step parseStep();
        void parseRequest(Step& step);
        void parseExpect(Step& step);
        void parseExtract(Step& step);
        void parseWait(Step& step);
        void parseWhen(Step& step);
    };
} // namespace zaplet::scenario::zsl

#endif // ZSL_PARSER_H
