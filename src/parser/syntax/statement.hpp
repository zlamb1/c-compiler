#pragma once

#include "as.hpp"
#include "expr.hpp"

struct Statement : public AbstractSyntax
{
};

struct Return : public Statement
{
    Expression* expr; 

    void accept(ASTVisitor* visitor) override
    {
        visitor->visitReturn(this); 
    }
};