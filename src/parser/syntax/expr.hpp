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

enum class UnaryOpType : int
{
    Negation,
    Complement,
    LogicalNegation
};

struct UnaryOp : public Expression
{
    UnaryOpType type; 
    Expression* expr; 

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitUnaryOp(this); 
    }
};

enum class BinaryOpType
{
    Addition,
    Subtraction,
    Multiplication,
    Division,
    LogicalOr,
    LogicalAnd,
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Remainder,
    BitwiseOr,
    BitwiseAnd,
    BitwiseXOR,
    BitwiseLeftShift,
    BitwiseRightShift
};

struct BinaryOp : public Expression
{
    BinaryOpType type; 
    Expression* lvalue; 
    Expression* rvalue; 

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitBinaryOp(this);
    }
};