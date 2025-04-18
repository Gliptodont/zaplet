/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/zsl/zsl_lexer.h"
#include "zaplet/logging/logger.h"

#include <algorithm>
#include <format>
#include <map>
#include <utility>

namespace zaplet::scenario::zsl
{
    std::string Token::toString() const
    {
        static const std::map<TokenType, std::string> typeNames = { { TokenType::SCENARIO, "SCENARIO" },
                                                                    { TokenType::DESCRIPTION, "DESCRIPTION" },
                                                                    { TokenType::ENV, "ENV" },
                                                                    { TokenType::REPEAT, "REPEAT" },
                                                                    { TokenType::CONTINUE_ON_ERROR, "CONTINUE_ON_ERROR" },
                                                                    { TokenType::STEP, "STEP" },
                                                                    { TokenType::GET, "GET" },
                                                                    { TokenType::POST, "POST" },
                                                                    { TokenType::PUT, "PUT" },
                                                                    { TokenType::DEL, "DELETE" },
                                                                    { TokenType::PATCH, "PATCH" },
                                                                    { TokenType::HEAD, "HEAD" },
                                                                    { TokenType::OPTIONS, "OPTIONS" },
                                                                    { TokenType::HEADER, "HEADER" },
                                                                    { TokenType::PARAM, "PARAM" },
                                                                    { TokenType::BODY, "BODY" },
                                                                    { TokenType::TIMEOUT, "TIMEOUT" },
                                                                    { TokenType::EXPECT, "EXPECT" },
                                                                    { TokenType::STATUS, "STATUS" },
                                                                    { TokenType::CONTAINS, "CONTAINS" },
                                                                    { TokenType::EXTRACT, "EXTRACT" },
                                                                    { TokenType::FROM, "FROM" },
                                                                    { TokenType::WAIT, "WAIT" },
                                                                    { TokenType::WHEN, "WHEN" },
                                                                    { TokenType::LEFT_BRACE, "LEFT_BRACE" },
                                                                    { TokenType::RIGHT_BRACE, "RIGHT_BRACE" },
                                                                    { TokenType::EQUALS, "EQUALS" },
                                                                    { TokenType::COLON, "COLON" },
                                                                    { TokenType::STRING, "STRING" },
                                                                    { TokenType::NUMBER, "NUMBER" },
                                                                    { TokenType::IDENTIFIER, "IDENTIFIER" },
                                                                    { TokenType::BOOLEAN, "BOOLEAN" },
                                                                    { TokenType::COMMENT, "COMMENT" },
                                                                    { TokenType::END_OF_FILE, "EOF" },
                                                                    { TokenType::UNKNOWN, "UNKNOWN" } };

        auto typeName = typeNames.find(type);
        if (typeName != typeNames.end())
        {
            return std::format("Token({}, '{}', line {})", typeName->second, value, line);
        }
        else
        {
            return std::format("Token({}, '{}', line {})", static_cast<int>(type), value, line);
        }
    }

    Lexer::Lexer(std::string source)
        : m_source(std::move(source))
    {
    }

    std::vector<Token> Lexer::scanTokens()
    {
        while (!isAtEnd())
        {
            m_start = m_current;
            scanToken();
        }

        m_tokens.emplace_back(TokenType::END_OF_FILE, "", m_line);
        return m_tokens;
    }

    void Lexer::scanToken()
    {
        char c = advance();

        switch (c)
        {
            // Single-character tokens
        case '{':
            addToken(TokenType::LEFT_BRACE);
            break;
        case '}':
            addToken(TokenType::RIGHT_BRACE);
            break;
        case '=':
            addToken(TokenType::EQUALS);
            break;
        case ':':
            addToken(TokenType::COLON);
            break;

            // Whitespace
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;

        case '\n':
            m_line++;
            break;

            // Comments
        case '#':
            comment();
            break;

            // String literals
        case '"':
            string('"');
            break;
        case '\'':
            string('\'');
            break;

        default:
            if (isDigit(c))
            {
                number();
            }
            else if (isAlpha(c))
            {
                identifier();
            }
            else
            {
                // Unexpected character
                addToken(TokenType::UNKNOWN);
                LOG_WARNING_FMT("Unexpected character at line {}: '{}'", m_line, c);
            }
            break;
        }
    }

    void Lexer::string(char quote)
    {
        while (peek() != quote && !isAtEnd())
        {
            if (peek() == '\n')
            {
                m_line++;
            }
            advance();
        }

        if (isAtEnd())
        {
            LOG_ERROR_FMT("Unterminated string at line {}", m_line);
            addToken(TokenType::UNKNOWN);
            return;
        }

        advance();

        std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
        addToken(TokenType::STRING, value);
    }

    void Lexer::number()
    {
        while (isDigit(peek()))
        {
            advance();
        }

        if (peek() == '.' && isDigit(peekNext()))
        {
            advance();

            while (isDigit(peek()))
            {
                advance();
            }
        }

        std::string value = m_source.substr(m_start, m_current - m_start);
        addToken(TokenType::NUMBER, value);
    }

    void Lexer::identifier()
    {
        while (isAlphaNumeric(peek()) || peek() == '_' || peek() == '-' || peek() == '.' || peek() == '$' || peek() == '{' || peek() == '}')
        {
            advance();
        }

        std::string text = m_source.substr(m_start, m_current - m_start);

        static const std::map<std::string, TokenType> keywords = { { "SCENARIO", TokenType::SCENARIO },
                                                                   { "DESCRIPTION", TokenType::DESCRIPTION },
                                                                   { "ENV", TokenType::ENV },
                                                                   { "REPEAT", TokenType::REPEAT },
                                                                   { "CONTINUE_ON_ERROR", TokenType::CONTINUE_ON_ERROR },
                                                                   { "STEP", TokenType::STEP },
                                                                   { "GET", TokenType::GET },
                                                                   { "POST", TokenType::POST },
                                                                   { "PUT", TokenType::PUT },
                                                                   { "DELETE", TokenType::DEL },
                                                                   { "PATCH", TokenType::PATCH },
                                                                   { "HEAD", TokenType::HEAD },
                                                                   { "OPTIONS", TokenType::OPTIONS },
                                                                   { "HEADER", TokenType::HEADER },
                                                                   { "PARAM", TokenType::PARAM },
                                                                   { "BODY", TokenType::BODY },
                                                                   { "TIMEOUT", TokenType::TIMEOUT },
                                                                   { "EXPECT", TokenType::EXPECT },
                                                                   { "STATUS", TokenType::STATUS },
                                                                   { "CONTAINS", TokenType::CONTAINS },
                                                                   { "EXTRACT", TokenType::EXTRACT },
                                                                   { "FROM", TokenType::FROM },
                                                                   { "WAIT", TokenType::WAIT },
                                                                   { "WHEN", TokenType::WHEN },
                                                                   { "TRUE", TokenType::BOOLEAN },
                                                                   { "FALSE", TokenType::BOOLEAN } };

        std::string upperText = text;
        std::transform(upperText.begin(), upperText.end(), upperText.begin(), ::toupper);

        auto it = keywords.find(upperText);
        if (it != keywords.end())
        {
            addToken(it->second, text);
        }
        else
        {
            addToken(TokenType::IDENTIFIER, text);
        }
    }

    void Lexer::comment()
    {
        while (peek() != '\n' && !isAtEnd())
        {
            advance();
        }

        std::string comment = m_source.substr(m_start + 1, m_current - m_start - 1);
        addToken(TokenType::COMMENT, comment);
    }

    bool Lexer::isAtEnd() const
    {
        return m_current >= static_cast<int>(m_source.length());
    }

    char Lexer::advance()
    {
        return m_source[m_current++];
    }

    void Lexer::addToken(TokenType type)
    {
        addToken(type, m_source.substr(m_start, m_current - m_start));
    }

    void Lexer::addToken(TokenType type, const std::string& text)
    {
        m_tokens.emplace_back(type, text, m_line);
    }

    bool Lexer::match(char expected)
    {
        if (isAtEnd() || m_source[m_current] != expected)
        {
            return false;
        }

        m_current++;
        return true;
    }

    char Lexer::peek() const
    {
        if (isAtEnd())
        {
            return '\0';
        }
        return m_source[m_current];
    }

    char Lexer::peekNext() const
    {
        if (m_current + 1 >= static_cast<int>(m_source.length()))
        {
            return '\0';
        }

        return m_source[m_current + 1];
    }

    bool Lexer::isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool Lexer::isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool Lexer::isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }
} // namespace zaplet::scenario::zsl