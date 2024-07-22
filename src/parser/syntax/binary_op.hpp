#pragma once

#include "expr.hpp"

enum class BinaryOpType
{
    Addition,
    Subtraction,
    Multiplication,
    Division,
    LogicalOr,
    LogicalAnd,
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Remainder,
    BitwiseOr,
    BitwiseAnd,
    BitwiseXOR,
    BitwiseLeftShift,
    BitwiseRightShift,
    Comma
};

struct BinaryOp : public Expression
{
    BinaryOpType opType; 
    Expression::Ref lvalue; 
    Expression::Ref rvalue; 

    BinaryOp(BinaryOpType opType, Expression::Ref lvalue, Expression::Ref rvalue) : Expression(SyntaxType::BinaryOp), opType(opType), lvalue(lvalue), rvalue(rvalue)
    {
    }

    typedef std::shared_ptr<BinaryOp> Ref;
};