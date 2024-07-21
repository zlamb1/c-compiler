#pragma once

#include "visitor.hpp"

enum class SyntaxType : int
{
    Program,
    Function,
    // Statements
    StatementExpression,
    Declaration,
    Return,
    // Expressions
    UnaryOp,
    BinaryOp,
    IntConstant,
    VariableRef,
    Assignment
};

class AbstractSyntax
{
public:
    AbstractSyntax(SyntaxType type) : _type(type) 
    {
    }
    
    virtual ~AbstractSyntax() = default;

    SyntaxType type()
    {
        return _type; 
    }

    virtual void Accept(ASTVisitor* visitor) = 0;
private:
    SyntaxType _type; 
};