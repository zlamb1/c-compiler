#pragma once

#include "statement.hpp"

struct StatementExpression : public Statement
{
    Expression::Ref expression; 

    StatementExpression(Expression::Ref expression) : Statement(SyntaxType::StatementExpression), expression(expression)
    {
    }

    typedef std::shared_ptr<StatementExpression> Ref;
};