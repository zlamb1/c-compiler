#pragma once

#include "parser/syntax/statement/statement.hpp"
#include "parser/syntax/statement/statement_expr.hpp"

struct ForStatement : public Statement
{
    Expression::Ref expression; 
    Expression::Ref condition;
    Expression::Ref post_expression;
    Statement::Ref body; 

    ForStatement(Expression::Ref expression, Expression::Ref condition, Expression::Ref post_expression, Statement::Ref body) : 
        Statement(SyntaxType::For), expression(expression), condition(condition), post_expression(post_expression), body(body)
    {
    }

    typedef std::shared_ptr<ForStatement> Ref; 
};