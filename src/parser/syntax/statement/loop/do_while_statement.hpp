#pragma once

#include "parser/syntax/statement/statement.hpp"

struct DoWhileStatement : public Statement
{
    Statement::Ref body; 
    Expression::Ref condition;

    DoWhileStatement(Statement::Ref body, Expression::Ref condition) : Statement(SyntaxType::DoWhile), body(body), condition(condition)
    {
    }

    typedef std::shared_ptr<DoWhileStatement> Ref; 
};