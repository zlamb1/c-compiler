#pragma once

#include "visitor.hpp"

enum class SyntaxType : int
{
    Program,
    Function,
    // Statements
    Return,
    // Expressions
    UnaryOp,
    BinaryOp,
    IntConstant
};

struct AbstractSyntax
{
    SyntaxType type; 

    AbstractSyntax(SyntaxType type) : type(type) 
    {
    }
    
    virtual ~AbstractSyntax() = default;

    virtual void Accept(ASTVisitor* visitor) = 0;
};