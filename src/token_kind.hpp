#pragma once

enum class TokenKind : int
{
    // Values
    Identifier,

    // Keywords
    Package,
    Struct,
    Class,
    Include,
    Enum,
    For,
    While,
    Fn,

    // Delimiters
    Quote,
    SingleQuote,
    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    SemiColon,
    ColonColon,

    // Operators
    Dot,
    DotDot,
    Equal,
    EqualEqual,
    Not,
    NotEqual,
    Plus,
    PlusPlus,
    Minus,
    MinusMinus,
    Slash,
    Asterisk,
    Percent
};