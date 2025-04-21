/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/scenario/zsl/zsl_parser.h"
#include "zaplet/logging/logger.h"

#include <format>
#include <sstream>

namespace zaplet::scenario::zsl
{
    Parser::Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens))
    {
    }

    std::optional<Scenario> Parser::parse()
    {
        try
        {
            m_errors.clear();
            m_current = 0;
            m_scenario = Scenario();

            parseScenario();

            if (!m_errors.empty())
            {
                return std::nullopt;
            }

            return m_scenario;
        } catch (const std::exception& e)
        {
            m_errors.push_back(std::format("Exception during parsing: {}", e.what()));
            return std::nullopt;
        }
    }

    const std::vector<std::string>& Parser::getErrors() const
    {
        return m_errors;
    }

    void Parser::dumpToken(const Token& token, const std::string& context)
    {
        LOG_DEBUG_FMT("{}:  Type={}, Value='{}', Line={}", context, static_cast<int>(token.type), token.value, token.line);
    }

    void Parser::dumpCurrentToken(const std::string& context)
    {
        dumpToken(peek(), context);
    }

    bool Parser::match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::match(const std::initializer_list<TokenType>& types)
    {
        for (auto type : types)
        {
            if (match(type))
            {
                return true;
            }
        }
        return false;
    }

    bool Parser::check(TokenType type) const
    {
        if (isAtEnd())
        {
            return false;
        }
        return peek().type == type;
    }

    bool Parser::check(const std::initializer_list<TokenType>& types) const
    {
        if (isAtEnd())
        {
            return false;
        }

        for (auto type : types)
        {
            if (peek().type == type)
            {
                return true;
            }
        }
        return false;
    }

    const Token& Parser::previous() const
    {
        return m_tokens.at(m_current - 1);
    }

    const Token& Parser::advance()
    {
        if (!isAtEnd())
        {
            m_current++;
        }
        return previous();
    }

    const Token& Parser::peek() const
    {
        return m_tokens.at(m_current);
    }

    bool Parser::isAtEnd() const
    {
        return peek().type == TokenType::END_OF_FILE;
    }

    void Parser::error(const std::string& message)
    {
        std::string errorMsg = std::format("Line {}: {}", peek().line, message);
        m_errors.push_back(errorMsg);
        LOG_ERROR(errorMsg);
    }

    void Parser::parseScenario()
    {
        LOG_DEBUG("Starting scenario parsing");

        enum class ParseState
        {
            EXPECTING_SCENARIO,
            EXPECTING_METADATA_OR_STEP,
            DONE
        };

        ParseState state = ParseState::EXPECTING_SCENARIO;

        while (!isAtEnd() && state != ParseState::DONE)
        {
            dumpCurrentToken("Current token");

            switch (state)
            {
            case ParseState::EXPECTING_SCENARIO:
                {
                    while (match(TokenType::COMMENT))
                    {
                    }

                    if (!match(TokenType::SCENARIO))
                    {
                        error("Expected 'SCENARIO' keyword at the beginning of the file");
                        state = ParseState::DONE;
                        break;
                    }

                    LOG_DEBUG("Found SCENARIO keyword");

                    if (!match(TokenType::STRING))
                    {
                        error("Expected scenario name as a string after SCENARIO keyword");
                        state = ParseState::DONE;
                        break;
                    }

                    LOG_DEBUG_FMT("Found scenario name: {}", previous().value);
                    m_scenario.setName(previous().value);

                    state = ParseState::EXPECTING_METADATA_OR_STEP;
                }
                break;

            case ParseState::EXPECTING_METADATA_OR_STEP:
                {
                    if (match(TokenType::COMMENT))
                    {
                        break;
                    }

                    if (match(TokenType::DESCRIPTION))
                    {
                        LOG_DEBUG("Parsing DESCRIPTION");

                        if (!match(TokenType::STRING))
                        {
                            error("Expected string after DESCRIPTION keyword");
                            state = ParseState::DONE;
                            break;
                        }

                        m_scenario.setDescription(previous().value);
                    }
                    else if (match(TokenType::ENV))
                    {
                        LOG_DEBUG("Parsing ENV");

                        if (!match(TokenType::IDENTIFIER))
                        {
                            error("Expected environment variable name after ENV keyword");
                            state = ParseState::DONE;
                            break;
                        }

                        std::string name = previous().value;
                        LOG_DEBUG_FMT("ENV name: {}", name);

                        if (!match(TokenType::EQUALS))
                        {
                            error("Expected '=' after environment variable name");
                            state = ParseState::DONE;
                            break;
                        }

                        if (!match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::URL }))
                        {
                            error("Expected value after '='");
                            state = ParseState::DONE;
                            break;
                        }

                        std::string value = previous().value;
                        LOG_DEBUG_FMT("ENV value: {}", value);

                        m_scenario.addEnvironmentVariable(name, value);
                    }
                    else if (match(TokenType::REPEAT))
                    {
                        LOG_DEBUG("Parsing REPEAT");

                        if (!match(TokenType::NUMBER))
                        {
                            error("Expected number after REPEAT keyword");
                            state = ParseState::DONE;
                            break;
                        }

                        try
                        {
                            int repeatCount = std::stoi(previous().value);
                            if (repeatCount < 0)
                            {
                                m_scenario.setRepeatCount(std::nullopt);
                            }
                            else
                            {
                                m_scenario.setRepeatCount(repeatCount);
                            }
                        } catch (const std::exception& e)
                        {
                            error(std::format("Invalid repeat count: {}", e.what()));
                            state = ParseState::DONE;
                            break;
                        }
                    }
                    else if (match(TokenType::CONTINUE_ON_ERROR))
                    {
                        LOG_DEBUG("Parsing CONTINUE_ON_ERROR");

                        if (!match(TokenType::BOOLEAN))
                        {
                            error("Expected TRUE or FALSE after CONTINUE_ON_ERROR keyword");
                            state = ParseState::DONE;
                            break;
                        }

                        m_scenario.setContinueOnError(previous().value == "TRUE" || previous().value == "true");
                    }
                    else if (match(TokenType::STEP))
                    {
                        LOG_DEBUG("Parsing STEP");

                        Step step = parseStep();
                        m_scenario.addStep(step);
                    }
                    else
                    {
                        error(std::format("Unexpected token: {}", peek().value));
                        advance();
                    }
                }
                break;

            case ParseState::DONE:
                break;
            }
        }

        LOG_DEBUG("Finished scenario parsing");
    }

    void Parser::parseMetadata(TokenType metadataType)
    {
        switch (metadataType)
        {
        case TokenType::DESCRIPTION:
            if (!match(TokenType::STRING))
            {
                error("Expected string after DESCRIPTION keyword");
                return;
            }
            m_scenario.setDescription(previous().value);
            break;

        case TokenType::ENV:
            {
                if (!match(TokenType::IDENTIFIER))
                {
                    error("Expected environment variable name after ENV keyword");
                    return;
                }
                std::string name = previous().value;

                if (!match(TokenType::EQUALS))
                {
                    error("Expected '=' after environment variable name");
                    return;
                }

                if (!match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::URL }))
                {
                    error("Expected value after '='");
                    return;
                }
                std::string value = previous().value;

                m_scenario.addEnvironmentVariable(name, value);
            }
            break;

        case TokenType::REPEAT:
            if (!match(TokenType::NUMBER))
            {
                error("Expected number after REPEAT keyword");
                return;
            }
            try
            {
                int repeatCount = std::stoi(previous().value);

                if (repeatCount == -1)
                {
                    m_scenario.setRepeatCount(std::nullopt);
                    LOG_DEBUG("Setting infinite repeat (null)");
                }
                else if (repeatCount < 0)
                {
                    error("Repeat count must be positive or -1 (for infinite)");
                    return;
                }
                else
                {
                    m_scenario.setRepeatCount(repeatCount);
                    LOG_DEBUG_FMT("Setting repeat count: {}", repeatCount);
                }
            } catch (const std::exception& e)
            {
                error(std::format("Invalid repeat count: {}", e.what()));
            }
            break;

        case TokenType::CONTINUE_ON_ERROR:
            if (!match(TokenType::BOOLEAN))
            {
                error("Expected TRUE or FALSE after CONTINUE_ON_ERROR keyword");
                return;
            }
            m_scenario.setContinueOnError(previous().value == "TRUE" || previous().value == "true");
            break;

        default:
            error(std::format("Unexpected metadata token: {}", static_cast<int>(metadataType)));
            break;
        }
    }

    Step Parser::parseStep()
    {
        LOG_DEBUG("Starting step parsing");
        Step step;

        if (!match(TokenType::STRING))
        {
            error("Expected step name as a string after STEP keyword");
            return step;
        }

        LOG_DEBUG_FMT("Found step name: {}", previous().value);
        step.setDescription(previous().value);

        if (!match(TokenType::LEFT_BRACE))
        {
            error("Expected '{' after step name");
            return step;
        }

        LOG_DEBUG("Found step opening brace");

        while (!isAtEnd() && !check(TokenType::RIGHT_BRACE))
        {
            dumpCurrentToken("Step content token");

            if (match(TokenType::COMMENT))
            {
                continue;
            }
            else if (match(
                         { TokenType::GET,
                           TokenType::POST,
                           TokenType::PUT,
                           TokenType::DEL,
                           TokenType::PATCH,
                           TokenType::HEAD,
                           TokenType::OPTIONS }))
            {
                LOG_DEBUG_FMT("Parsing HTTP method: {}", previous().value);
                parseRequest(step);
            }
            else if (match(TokenType::EXPECT))
            {
                LOG_DEBUG("Parsing EXPECT");
                parseExpect(step);
            }
            else if (match(TokenType::EXTRACT))
            {
                LOG_DEBUG("Parsing EXTRACT");
                parseExtract(step);
            }
            else if (match(TokenType::WAIT))
            {
                LOG_DEBUG("Parsing WAIT");
                parseWait(step);
            }
            else if (match(TokenType::WHEN))
            {
                LOG_DEBUG("Parsing WHEN");
                parseWhen(step);
            }
            else
            {
                error(std::format("Unexpected token in step: {}", peek().value));
                advance();
            }
        }

        if (!match(TokenType::RIGHT_BRACE))
        {
            error("Expected '}' to close step definition");
        }
        else
        {
            LOG_DEBUG("Found step closing brace");
        }

        LOG_DEBUG("Finished step parsing");
        return step;
    }

    void Parser::parseRequest(Step& step)
    {
        LOG_DEBUG("Starting request parsing");

        http::Request request;

        std::string method = previous().value;
        request.setMethod(method);
        LOG_DEBUG_FMT("HTTP method: {}", method);

        std::stringstream urlStream;

        if (match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::URL, TokenType::VARIABLE }))
        {
            urlStream << previous().value;
            LOG_DEBUG_FMT("URL base part: {}", previous().value);

            while (match(TokenType::URL) || (check(TokenType::IDENTIFIER) && peek().value.find("/") == 0))
            {
                urlStream << previous().value;
                LOG_DEBUG_FMT("URL path part: {}", previous().value);
            }
        }
        else
        {
            error("Expected URL after HTTP method");
            return;
        }

        request.setUrl(urlStream.str());
        LOG_DEBUG_FMT("Complete URL: {}", urlStream.str());

        while (!isAtEnd() && !check(TokenType::RIGHT_BRACE) &&
               !check(
                   { TokenType::GET,
                     TokenType::POST,
                     TokenType::PUT,
                     TokenType::DEL,
                     TokenType::PATCH,
                     TokenType::HEAD,
                     TokenType::OPTIONS,
                     TokenType::EXPECT,
                     TokenType::EXTRACT,
                     TokenType::WAIT,
                     TokenType::WHEN }))
        {
            dumpCurrentToken("Request parameter token");

            if (match(TokenType::COMMENT))
            {
                continue;
            }
            else if (match(TokenType::HEADER))
            {
                LOG_DEBUG("Parsing header");

                if (!match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::VARIABLE }))
                {
                    error("Expected header name");
                    return;
                }

                std::string name = previous().value;
                LOG_DEBUG_FMT("Header name: {}", name);

                if (!match(TokenType::COLON))
                {
                    error("Expected ':' after header name");
                    return;
                }

                if (!match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::VARIABLE }))
                {
                    error("Expected header value");
                    return;
                }

                std::string value = previous().value;
                LOG_DEBUG_FMT("Header value: {}", value);

                request.addHeader(name, value);
            }
            else if (match(TokenType::PARAM))
            {
                LOG_DEBUG("Parsing param");

                if (!match({ TokenType::IDENTIFIER, TokenType::VARIABLE }))
                {
                    error("Expected parameter name");
                    return;
                }

                std::string name = previous().value;
                LOG_DEBUG_FMT("Param name: {}", name);

                if (!match(TokenType::EQUALS))
                {
                    error("Expected '=' after parameter name");
                    return;
                }

                if (!match({ TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::VARIABLE }))
                {
                    error("Expected parameter value");
                    return;
                }

                std::string value = previous().value;
                LOG_DEBUG_FMT("Param value: {}", value);

                request.addQueryParam(name, value);
            }
            else if (match(TokenType::BODY))
            {
                LOG_DEBUG("Parsing body");

                if (match({ TokenType::STRING, TokenType::VARIABLE }))
                {
                    LOG_DEBUG_FMT("Body as string/variable: {}", previous().value);
                    request.setBody(previous().value);
                }
                else if (match(TokenType::LEFT_BRACE))
                {
                    LOG_DEBUG("Body as JSON object");
                    std::stringstream bodyJson;
                    bodyJson << "{";

                    int braceLevel = 1;
                    bool needComma = false;

                    while (braceLevel > 0 && !isAtEnd())
                    {
                        if (match(TokenType::COMMENT))
                        {
                            continue;
                        }
                        else if (match(TokenType::RIGHT_BRACE))
                        {
                            braceLevel--;
                            if (braceLevel > 0)
                            {
                                bodyJson << "}";
                            }
                        }
                        else if (match(TokenType::LEFT_BRACE))
                        {
                            braceLevel++;
                            bodyJson << "{";
                        }
                        else if (match({ TokenType::IDENTIFIER, TokenType::VARIABLE }))
                        {
                            if (needComma)
                                bodyJson << ", ";

                            std::string key = previous().value;
                            if (previous().type == TokenType::IDENTIFIER)
                            {
                                bodyJson << "\"" << key << "\"";
                            }
                            else
                            {
                                bodyJson << key;
                            }

                            if (!match(TokenType::COLON))
                            {
                                error("Expected ':' after JSON key");
                                return;
                            }

                            bodyJson << ": ";

                            if (match(TokenType::STRING))
                            {
                                bodyJson << "\"" << previous().value << "\"";
                                needComma = true;
                            }
                            else if (match(TokenType::NUMBER))
                            {
                                bodyJson << previous().value;
                                needComma = true;
                            }
                            else if (match(TokenType::BOOLEAN))
                            {
                                bodyJson << (previous().value == "TRUE" || previous().value == "true" ? "true" : "false");
                                needComma = true;
                            }
                            else if (match(TokenType::VARIABLE))
                            {
                                bodyJson << previous().value;
                                needComma = true;
                            }
                            else
                            {
                                error("Expected value in JSON object");
                                return;
                            }
                        }
                        else
                        {
                            error(std::format("Unexpected token in JSON body: {}", peek().value));
                            advance();
                        }
                    }

                    bodyJson << "}";
                    request.setBody(bodyJson.str());
                }
                else
                {
                    error("Expected string or JSON object after BODY");
                    return;
                }
            }
            else if (match(TokenType::TIMEOUT))
            {
                LOG_DEBUG("Parsing timeout");

                if (!match(TokenType::NUMBER))
                {
                    error("Expected number after TIMEOUT");
                    return;
                }

                try
                {
                    int timeout = std::stoi(previous().value);
                    LOG_DEBUG_FMT("Timeout value: {}", timeout);
                    request.setTimeout(timeout);
                } catch (const std::exception& e)
                {
                    error(std::format("Invalid timeout value: {}", e.what()));
                }
            }
            else
            {
                error(std::format("Unexpected token in request: {}", peek().value));
                advance();
            }
        }

        step.setRequest(request);
        LOG_DEBUG("Finished request parsing");
    }

    void Parser::parseExpect(Step& step)
    {
        if (!step.getExpectedResponse())
        {
            http::Response response;
            step.setExpectedResponse(response);
        }

        http::Response& response = const_cast<http::Response&>(step.getExpectedResponse().value());

        if (match(TokenType::STATUS))
        {
            if (!match(TokenType::NUMBER))
            {
                error("Expected status code as number after STATUS");
                return;
            }

            try
            {
                int statusCode = std::stoi(previous().value);
                response.setStatusCode(statusCode);
            } catch (const std::exception& e)
            {
                error(std::format("Invalid status code: {}", e.what()));
            }
        }
        else if (match(TokenType::HEADER))
        {
            if (!match({ TokenType::STRING, TokenType::IDENTIFIER }))
            {
                error("Expected header name");
                return;
            }

            std::string name = previous().value;

            if (!match(TokenType::COLON))
            {
                error("Expected ':' after header name");
                return;
            }

            if (!match({ TokenType::STRING, TokenType::IDENTIFIER }))
            {
                error("Expected header value");
                return;
            }

            std::string value = previous().value;
            response.addHeader(name, value);
        }
        else if (match(TokenType::BODY))
        {
            if (match(TokenType::CONTAINS))
            {
                if (!match(TokenType::STRING))
                {
                    error("Expected string after CONTAINS");
                    return;
                }

                response.setBody(previous().value);
            }
            else if (match(TokenType::STRING))
            {
                response.setBody(previous().value);
            }
            else if (match(TokenType::LEFT_BRACE))
            {
                std::stringstream bodyJson;
                bodyJson << "{";

                int braceLevel = 1;
                bool needComma = false;

                while (braceLevel > 0 && !isAtEnd())
                {
                    if (match(TokenType::COMMENT))
                    {
                        continue;
                    }
                    else if (match(TokenType::RIGHT_BRACE))
                    {
                        braceLevel--;
                        if (braceLevel > 0)
                        {
                            bodyJson << "}";
                        }
                    }
                    else if (match(TokenType::LEFT_BRACE))
                    {
                        braceLevel++;
                        bodyJson << "{";
                    }
                    else if (match(TokenType::IDENTIFIER))
                    {
                        if (needComma)
                            bodyJson << ", ";
                        bodyJson << "\"" << previous().value << "\"";

                        if (!match(TokenType::COLON))
                        {
                            error("Expected ':' after JSON key");
                            return;
                        }

                        bodyJson << ": ";

                        if (match(TokenType::STRING))
                        {
                            bodyJson << "\"" << previous().value << "\"";
                            needComma = true;
                        }
                        else if (match(TokenType::NUMBER))
                        {
                            bodyJson << previous().value;
                            needComma = true;
                        }
                        else if (match(TokenType::BOOLEAN))
                        {
                            bodyJson << (previous().value == "TRUE" || previous().value == "true" ? "true" : "false");
                            needComma = true;
                        }
                        else
                        {
                            error("Expected value in JSON object");
                            return;
                        }
                    }
                    else
                    {
                        error(std::format("Unexpected token in JSON body: {}", peek().value));
                        advance();
                    }
                }

                bodyJson << "}";
                response.setBody(bodyJson.str());
            }
            else
            {
                error("Expected STRING, JSON, or CONTAINS after BODY");
                return;
            }
        }
        else
        {
            error("Expected STATUS, HEADER, or BODY after EXPECT");
            return;
        }
    }

    void Parser::parseExtract(Step& step)
    {
        if (!match(TokenType::IDENTIFIER))
        {
            error("Expected variable name after EXTRACT");
            return;
        }

        std::string variableName = previous().value;

        if (!match(TokenType::FROM))
        {
            error("Expected FROM after variable name");
            return;
        }

        // Parse extraction path
        if (!match(TokenType::IDENTIFIER))
        {
            error("Expected extraction path (e.g., body.data.id or header.Location)");
            return;
        }

        std::string path = previous().value;
        step.addVariable(variableName, path);
    }

    void Parser::parseWait(Step& step)
    {
        if (!match(TokenType::NUMBER))
        {
            error("Expected wait duration in milliseconds");
            return;
        }

        try
        {
            int duration = std::stoi(previous().value);
            step.setDelay(std::chrono::milliseconds(duration));
        } catch (const std::exception& e)
        {
            error(std::format("Invalid wait duration: {}", e.what()));
        }
    }

    void Parser::parseWhen(Step& step)
    {
        std::stringstream conditionStream;

        while (!isAtEnd() && !check(TokenType::RIGHT_BRACE) &&
               !check(
                   { TokenType::GET,
                     TokenType::POST,
                     TokenType::PUT,
                     TokenType::DEL,
                     TokenType::PATCH,
                     TokenType::HEAD,
                     TokenType::OPTIONS,
                     TokenType::EXPECT,
                     TokenType::EXTRACT,
                     TokenType::WAIT }))
        {
            if (match(TokenType::COMMENT))
            {
                continue;
            }

            conditionStream << advance().value << " ";
        }

        std::string condition = conditionStream.str();
        if (!condition.empty())
        {
            size_t lastNonSpace = condition.find_last_not_of(" \t\n\r");
            if (lastNonSpace != std::string::npos)
            {
                condition = condition.substr(0, lastNonSpace + 1);
            }

            LOG_DEBUG_FMT("Parsed condition: {}", condition);
            step.setCondition(condition);
        }
        else
        {
            error("Empty condition after WHEN");
        }
    }
} // namespace zaplet::scenario::zsl