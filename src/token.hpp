#pragma once

#include <string>

#include "token_kind.hpp"

struct Token
{
    Token(TokenKind kind) : kind{kind}, value{}
    {
    }
    Token(TokenKind kind, const std::string& value) : kind{kind}, value{value}
    {
    }

    TokenKind kind;
    std::string value{}; 
};