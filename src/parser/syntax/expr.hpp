#pragma once

#include "as.hpp"

struct Expression : public AbstractSyntax
{
};

struct IntExpr : public Expression
{
    int value;
    void accept(ASTVisitor* visitor) override
    {
        visitor->visitIntExpr(this); 
    } 
};