#pragma once

#include <unordered_map>

enum class TokenKind : int
{
    Identifier,
    Keyword,
    IntConstant,
    LeftParenthesis,
    RightParenthesis,
    LeftBrace,
    RightBrace,
    Semicolon
};
