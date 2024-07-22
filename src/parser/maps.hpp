#pragma once

#include "lexer/token_kind.hpp"
#include "syntax/expr.hpp"
#include "syntax/assignment_op.hpp"

static std::unordered_map<TokenKind, UnaryOpType> TOKEN_TO_UNARY_TYPE
{
    { TokenKind::Minus, UnaryOpType::Negation },
    { TokenKind::Tilde, UnaryOpType::Complement },
    { TokenKind::Exclamation, UnaryOpType::LogicalNegation }
};

static std::unordered_map<TokenKind, BinaryOpType> TOKEN_TO_BINARY_TYPE
{
    { TokenKind::Plus, BinaryOpType::Addition },
    { TokenKind::Minus, BinaryOpType::Subtraction },
    { TokenKind::Asterisk, BinaryOpType::Multiplication },
    { TokenKind::Slash, BinaryOpType::Division },
    { TokenKind::Pipe, BinaryOpType::BitwiseOr },
    { TokenKind::DoublePipe, BinaryOpType::LogicalOr },
    { TokenKind::Ampersand, BinaryOpType::BitwiseAnd },
    { TokenKind::DoubleAmpersand, BinaryOpType::LogicalAnd },
    { TokenKind::DoubleEquals, BinaryOpType::Equal },
    { TokenKind::NotEqual, BinaryOpType::NotEqual },
    { TokenKind::LessThan, BinaryOpType::LessThan },
    { TokenKind::LessThanOrEqual, BinaryOpType::LessThanOrEqual },
    { TokenKind::GreaterThan, BinaryOpType::GreaterThan },
    { TokenKind::GreaterThanOrEqual, BinaryOpType::GreaterThanOrEqual },
    { TokenKind::Percent, BinaryOpType::Remainder },
    { TokenKind::Caret, BinaryOpType::BitwiseXOR },
    { TokenKind::LeftShift, BinaryOpType::BitwiseLeftShift },
    { TokenKind::RightShift, BinaryOpType::BitwiseRightShift }
};

static std::unordered_map<TokenKind, AssignmentOpType> TOKEN_TO_ASSIGNMENT_OP_TYPE
{
    { TokenKind::AddEquals, AssignmentOpType::Add },
    { TokenKind::MinusEquals, AssignmentOpType::Minus },
    { TokenKind::AsteriskEquals, AssignmentOpType::Multiplication },
    { TokenKind::SlashEquals, AssignmentOpType::Division },
    { TokenKind::PercentEquals, AssignmentOpType::Modulo },
    { TokenKind::LeftShiftEquals, AssignmentOpType::LeftShift },
    { TokenKind::RightShiftEquals, AssignmentOpType::RightShift },
    { TokenKind::OrEquals, AssignmentOpType::LogicalOr },
    { TokenKind::AndEquals, AssignmentOpType::LogicalAnd },
    { TokenKind::CaretEquals, AssignmentOpType::LogicalXOR }
};