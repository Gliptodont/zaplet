/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef ZSL_LEXER_H
#define ZSL_LEXER_H

#include "zaplet/scenario/step.h"

#include <filesystem>
#include <string>
#include <vector>

namespace zaplet::scenario::zsl
{
    enum class TokenType
    {
        // Keywords
        SCENARIO,
        DESCRIPTION,
        ENV,
        REPEAT,
        CONTINUE_ON_ERROR,
        STEP,
        GET,
        POST,
        PUT,
        DEL,
        PATCH,
        HEAD,
        OPTIONS,
        HEADER,
        PARAM,
        BODY,
        TIMEOUT,
        EXPECT,
        STATUS,
        CONTAINS,
        EXTRACT,
        FROM,
        WAIT,
        WHEN,

        // Punctuation
        LEFT_BRACE,
        RIGHT_BRACE,
        EQUALS,
        COLON,

        // Literals
        STRING,
        NUMBER,
        IDENTIFIER,
        BOOLEAN,

        // Special
        COMMENT,
        END_OF_FILE,
        UNKNOWN
    };

    struct Token
    {
        TokenType type;
        std::string value;
        int line;

        Token(TokenType type, std::string value, int line)
            : type(type)
            , value(std::move(value))
            , line(line)
        {
        }

        std::string toString() const;
    };

    class Lexer
    {
    public:
        explicit Lexer(std::string source);

        std::vector<Token> scanTokens();

    private:
        std::string m_source;
        std::vector<Token> m_tokens;
        int m_start = 0;
        int m_current = 0;
        int m_line = 1;

        void scanToken();
        void string(char quote);
        void number();
        void identifier();
        void comment();

        bool isAtEnd() const;
        char advance();
        void addToken(TokenType type);
        void addToken(TokenType type, const std::string& text);
        bool match(char expected);
        char peek() const;
        char peekNext() const;

        static bool isDigit(char c);
        static bool isAlpha(char c);
        static bool isAlphaNumeric(char c);
    };
} // namespace zaplet::scenario::zsl

#endif // ZSL_LEXER_H
