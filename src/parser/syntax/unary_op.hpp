#pragma once

#include "expr.hpp"

enum class UnaryOpType : int
{
    Negation,
    Complement,
    LogicalNegation
};

struct UnaryOp : public Expression
{
    UnaryOpType opType; 
    Expression::Ref expr; 

    UnaryOp(UnaryOpType opType, Expression::Ref expr) : Expression(SyntaxType::UnaryOp), opType(opType), expr(expr)
    {
    }

    typedef std::shared_ptr<UnaryOp> Ref;
};