#pragma once

#include "as.hpp"
#include "expr.hpp"

struct Statement : public AbstractSyntax
{
    Statement(SyntaxType type) : AbstractSyntax(type) 
    {
    }

    virtual ~Statement() = default; 
};

struct Return : public Statement
{
    Expression* expr; 

    Return(Expression* expr) : Statement(SyntaxType::Return), expr(expr)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitReturn(this); 
    }
};