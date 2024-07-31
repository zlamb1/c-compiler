#pragma once

#include "expr.hpp"

enum class UnaryOpType : int
{
    Negation,
    Complement,
    LogicalNegation,
    PrefixIncrement,
    PostfixIncrement,
    PrefixDecrement,
    PostfixDecrement
};

class UnaryOp : public Expression
{
public:
    Expression::Ref expr; 

    UnaryOp(UnaryOpType opType, Expression::Ref expr) : Expression(SyntaxType::UnaryOp), opType(opType), expr(expr)
    {
    }

    UnaryOpType OpType()
    {
        return opType; 
    }

    typedef std::shared_ptr<UnaryOp> Ref;
private:
    UnaryOpType opType; 
};