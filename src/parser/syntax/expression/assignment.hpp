#pragma once

#include "expression.hpp"

struct Assignment : public Expression
{
    std::string lvalue; 
    Expression::Ref rvalue; 

    Assignment(const std::string& lvalue, Expression::Ref rvalue) : Expression(SyntaxType::Assignment), lvalue(lvalue), rvalue(rvalue)
    {
    }

    typedef std::shared_ptr<Assignment> Ref;
};