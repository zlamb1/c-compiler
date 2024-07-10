#pragma once

enum class TokenKind : int
{
    Identifier,

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
    ArithemticIncrement,
    ArithmeticMinus,
    ArithmeticMinusEquals,
    ArithmeticDecrement,
    ArithmeticDivide,
    ArithmeticDivideEquals,
    ArithmeticMultiply,
    ArithmeticMultiplyEquals,
    ArithmeticModulo

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
    Dot,
    DotDot,
};