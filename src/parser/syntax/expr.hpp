#pragma once

#include "as.hpp"

struct Expression : public AbstractSyntax
{
};

struct IntExpr : public Expression
{
    int value;
    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitIntExpr(this); 
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

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitUnaryOp(this); 
    }
};