#pragma once

#include "parser/syntax/statement/decl.hpp"
#include "parser/syntax/statement/statement.hpp"
#include "parser/syntax/statement/statement_expr.hpp"

struct ForDeclStatement : public Statement
{
    Declaration::Ref declaration; 
    Expression::Ref condition;
    Expression::Ref post_expression;
    Statement::Ref body; 

    ForDeclStatement(Declaration::Ref declaration, Expression::Ref condition, Expression::Ref post_expression, Statement::Ref body) : 
        Statement(SyntaxType::ForDecl), declaration(declaration), condition(condition), post_expression(post_expression), body(body)
    {
    }

    typedef std::shared_ptr<ForDeclStatement> Ref; 
};