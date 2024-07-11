#pragma once

#include <unordered_map>

enum class TokenKind : int
{
    Identifier,

    // Numerical Constants
    IntegerLiteral,
    DoubleLiteral,
    FloatLiteral,

    // Constants
    True,
    False,

    // Misc. Keywords
    Package,
    Struct,
    Class,
    Include,
    Enum,
    Fn,

    // Control Flow
    For,
    While,
    If,
    Else,
    Continue,
    Break,
    Return,

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
    Colon,
    ColonColon,

    // Arithmetic Operators
    ArithmeticAdd,
    ArtihmeticAddEquals,
    ArithmeticIncrement,
    ArithmeticMinus,
    ArithmeticMinusEquals,
    ArithmeticDecrement,
    ArithmeticDivide,
    ArithmeticDivideEquals,
    ArithmeticMultiply,
    ArithmeticMultiplyEquals,
    ArithmeticModulo,

    // Comparison Operators
    CompareNot,
    CompareEqual,
    CompareNotEqual,
    CompareGreaterThan,
    CompareGreaterThanOrEqual,
    CompareLessThan,
    CompareLessThanOrEqual,
 
    // Logical Operators
    LogicalOr,
    LogicalAnd,

    // Bitwise Operators
    BitwiseOr,
    BitwiseOrEquals,
    BitwiseAnd,
    BitwiseAndEquals,
    BitwiseXOR,
    BitwiseLShift,
    BitwiseRShift,
    BitwiseNot,

    // Misc. Operators
    Assignment,
    Dot,
    DotDot,

    // Misc.
    None
};

static std::unordered_map<TokenKind, std::string> TOKEN_KIND_NAMES
{
    { TokenKind::Identifier, "Identifier" },

    // Numerical Constants
    { TokenKind::IntegerLiteral, "IntegerLiteral" },
    { TokenKind::DoubleLiteral, "DoubleLiteral" },
    { TokenKind::FloatLiteral, "FloatLiteral" },

    // Constants
    { TokenKind::True, "True" },
    { TokenKind::False, "False" },

    // Misc. Keywords
    { TokenKind::Package, "Package" },
    { TokenKind::Struct, "Struct" },
    { TokenKind::Class, "Class" },
    { TokenKind::Include, "Include" },
    { TokenKind::Enum, "Enum" },
    { TokenKind::Fn, "Fn" },

    // Control Flow
    { TokenKind::For, "For" },
    { TokenKind::While, "While" },
    { TokenKind::If, "If" },
    { TokenKind::Else, "Else" },
    { TokenKind::Continue, "Continue" },
    { TokenKind::Break, "Break" },
    { TokenKind::Return, "Return" },

    // Delimiters
    { TokenKind::Quote, "Quote" },
    { TokenKind::SingleQuote, "SingleQuote" },
    { TokenKind::LeftParen, "LeftParen" },
    { TokenKind::RightParen, "RightParen" },
    { TokenKind::LeftBracket, "LeftBracket" },
    { TokenKind::RightBracket, "RightBracket" },
    { TokenKind::LeftBrace, "LeftBrace" },
    { TokenKind::RightBrace, "RightBrace" },
    { TokenKind::SemiColon, "SemiColon" },
    { TokenKind::Colon, "Colon" },
    { TokenKind::ColonColon, "ColonColon" },

    // Arithmetic Operators
    { TokenKind::ArithmeticAdd, "ArithmeticAdd" },
    { TokenKind::ArtihmeticAddEquals, "ArithmeticAddEquals" },
    { TokenKind::ArithmeticIncrement, "ArimethicIncrement" },
    { TokenKind::ArithmeticMinus, "ArithmeticMinus" },
    { TokenKind::ArithmeticMinusEquals, "ArithmeticMinusEquals" },
    { TokenKind::ArithmeticDecrement, "ArithmeticDecrement" },
    { TokenKind::ArithmeticDivide, "ArithmeticDivide" },
    { TokenKind::ArithmeticDivideEquals, "ArithmeticDivideEquals" },
    { TokenKind::ArithmeticMultiply, "ArithmeticMultiply" },
    { TokenKind::ArithmeticMultiplyEquals, "ArithmeticMultiplyEquals" },
    { TokenKind::ArithmeticModulo, "ArithmeticModulo" },

    // Comparison Operators
    { TokenKind::CompareNot, "CompareNot" },
    { TokenKind::CompareEqual, "CompareEqual" },
    { TokenKind::CompareNotEqual, "CompareNotEqual" },
    { TokenKind::CompareGreaterThan, "CompareGreaterThan" },
    { TokenKind::CompareGreaterThanOrEqual, "CompareGreaterThanOrEqual" },
    { TokenKind::CompareLessThan, "CompareLessThan" },
    { TokenKind::CompareLessThanOrEqual, "CompareLessThanOrEqual" },
 
    // Logical Operators
    { TokenKind::LogicalOr, "LogicalOr" },
    { TokenKind::LogicalAnd, "LogicalAnd" },

    // Bitwise Operators
    { TokenKind::BitwiseOr, "BitwiseOr" },
    { TokenKind::BitwiseOrEquals, "BitwiseOrEquals" },
    { TokenKind::BitwiseAnd, "BitwiseAnd" },
    { TokenKind::BitwiseAndEquals, "BitwiseAndEquals" },
    { TokenKind::BitwiseXOR, "BitwiseXOR" },
    { TokenKind::BitwiseLShift, "BitwiseLShift" },
    { TokenKind::BitwiseRShift, "BitwiseRShift" },
    { TokenKind::BitwiseNot, "BitwiseNot" },

    // Misc. Operators
    { TokenKind::Assignment, "Assignment" },
    { TokenKind::Dot, "Dot" },
    { TokenKind::DotDot, "DotDot" },

    // Misc.
    { TokenKind::None, "None" }
};