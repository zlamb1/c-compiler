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
    Minus,
    Tilde,
    Exclamation,
    Plus,
    Asterisk,
    Slash,
    Ampersand,
    DoubleAmpersand,
    Pipe,
    DoublePipe,
    DoubleEquals,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Percent,
    Caret,
    LeftShift,
    RightShift,

    Comment,
    MultilineComment,
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
    { TokenKind::Minus, "-" },
    { TokenKind::Tilde, "~" },
    { TokenKind::Exclamation, "!" },
    { TokenKind::Plus, "+" },
    { TokenKind::Asterisk, "*" },
    { TokenKind::Slash, "/" },
    { TokenKind::Ampersand, "&" },
    { TokenKind::DoubleAmpersand, "&&" },
    { TokenKind::Pipe, "|" },
    { TokenKind::DoublePipe, "||" },
    { TokenKind::DoubleEquals, "==" },
    { TokenKind::NotEqual, "!=" },
    { TokenKind::LessThan, "<" },
    { TokenKind::LessThanOrEqual, "<=" },
    { TokenKind::GreaterThan, ">" },
    { TokenKind::GreaterThanOrEqual, ">=" },
    { TokenKind::Percent, "%" },
    { TokenKind::Caret, "^" },
    { TokenKind::LeftShift, "<<" },
    { TokenKind::RightShift, ">>" },
    { TokenKind::None, "None" }
};