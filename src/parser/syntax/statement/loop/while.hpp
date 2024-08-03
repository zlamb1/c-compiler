#pragma once

#include "parser/syntax/statement/statement.hpp"

struct WhileStatement : public Statement
{
    Expression::Ref condition;
    Statement::Ref body; 
    

    WhileStatement(Expression::Ref condition, Statement::Ref body) : Statement(SyntaxType::While), condition(condition), body(body)
    {
    }

    typedef std::shared_ptr<WhileStatement> Ref; 
};