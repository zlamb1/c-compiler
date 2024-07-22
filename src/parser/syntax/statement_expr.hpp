#pragma once

#include "statement.hpp"

struct StatementExpression : public Statement
{
    Expression::Ref expr; 

    StatementExpression(Expression::Ref expr) : Statement(SyntaxType::StatementExpression), expr(expr)
    {
    }

    typedef std::shared_ptr<StatementExpression> Ref;
};