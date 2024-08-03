#pragma once

#include "statement.hpp"

struct ReturnStatement : public Statement
{
    Expression::Ref expression; 

    ReturnStatement(Expression::Ref expression) : Statement(SyntaxType::Return), expression(expression)
    {
    }

    typedef std::shared_ptr<ReturnStatement> Ref;
};