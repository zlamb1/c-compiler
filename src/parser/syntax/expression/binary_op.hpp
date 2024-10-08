#pragma once

#include "expression.hpp"

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

class BinaryOp : public Expression
{
public:
    Expression::Ref lvalue; 
    Expression::Ref rvalue; 

    BinaryOp(BinaryOpType opType, Expression::Ref lvalue, Expression::Ref rvalue) : Expression(SyntaxType::BinaryOp), opType(opType), lvalue(lvalue), rvalue(rvalue)
    {
    }

    BinaryOpType OpType()
    {
        return opType; 
    }

    typedef std::shared_ptr<BinaryOp> Ref;
private:
    BinaryOpType opType; 
};