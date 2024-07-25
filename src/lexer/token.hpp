#pragma once

#include <string>

#include "token_kind.hpp"

struct Token
{
    Token(TokenKind kind, size_t line, size_t position) : kind{kind}, value(), line(line), position(position)
    {
    }
    Token(TokenKind kind, const std::string& value, size_t line, size_t position) : kind(kind), value(value), line(line), position(position)
    {
    }

    TokenKind kind;
    std::string value{}; 
    // metadata
    size_t line, position; 
};