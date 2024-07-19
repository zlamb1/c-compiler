#pragma once

#include "as.hpp"
#include "expr.hpp"

struct Statement : public AbstractSyntax
{
};

struct Return : public Statement
{
    Expression* expr; 

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitReturn(this); 
    }
};