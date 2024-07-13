#pragma once

#include <unordered_map>

enum class TokenKind : int
{
    Identifier,
    Keyword,
    IntConstant,
    HexConstant,
    LeftParenthesis,
    RightParenthesis,
    LeftBrace,
    RightBrace,
    Semicolon,

    None
};

static std::unordered_map<TokenKind, std::string> TOKEN_KIND_NAMES
{
    { TokenKind::Identifier, "Identifier" },
    { TokenKind::Keyword, "Keyword" },
    { TokenKind::IntConstant, "IntConstant" },
    { TokenKind::HexConstant, "HexConstant" },
    { TokenKind::LeftParenthesis, "(" },
    { TokenKind::RightParenthesis, ")" },
    { TokenKind::LeftBrace, "{" },
    { TokenKind::RightBrace, "}" },
    { TokenKind::Semicolon, ";" },
    { TokenKind::None, "None" }
};