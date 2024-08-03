#pragma once

#include "statement.hpp"

struct Return : public Statement
{
    Expression::Ref expr; 

    Return(Expression::Ref expr) : Statement(SyntaxType::Return), expr(expr)
    {
    }

    typedef std::shared_ptr<Return> Ref;
};