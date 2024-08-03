#pragma once

#include "expression.hpp"

struct TernaryOp : public Expression
{
    Expression::Ref condition, lvalue, rvalue;

    TernaryOp(Expression::Ref condition, Expression::Ref lvalue, Expression::Ref rvalue) : Expression(SyntaxType::TernaryOp), condition(condition),
        lvalue(lvalue), rvalue(rvalue)
    {
    }

    typedef std::shared_ptr<TernaryOp> Ref; 
};