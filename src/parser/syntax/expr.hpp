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

enum class UnaryType : int
{
    Negation,
    BWComplement,
    LogicalNegation
};

struct UnaryOp : public Expression
{
    UnaryType type; 
    Expression* expr; 

    void accept(ASTVisitor* visitor) override
    {
        visitor->visitUnaryOp(this); 
    }
};