#pragma once

#include "as.hpp"

struct Expression : public AbstractSyntax
{
    Expression(SyntaxType type) : AbstractSyntax(type) 
    {
    }
    virtual ~Expression() = default; 
};

struct IntConstant : public Expression
{
    int value;

    IntConstant(int value) : Expression(SyntaxType::IntConstant), value(value)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitIntConstant(this); 
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
    UnaryOpType opType; 
    Expression* expr; 

    UnaryOp(UnaryOpType opType, Expression* expr) : Expression(SyntaxType::UnaryOp), opType(opType), expr(expr)
    {
    }

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
    BinaryOpType opType; 
    Expression* lvalue; 
    Expression* rvalue; 

    BinaryOp(BinaryOpType opType, Expression* lvalue, Expression* rvalue) : Expression(SyntaxType::BinaryOp), opType(opType), lvalue(lvalue), rvalue(rvalue)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitBinaryOp(this);
    }
};

struct VariableRef : public Expression
{
    std::string name; 

    VariableRef(const std::string& name) : Expression(SyntaxType::VariableRef), name(name)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitVariableRef(this); 
    }
};

struct Assignment : public Expression
{
    std::string lvalue; 
    Expression* rvalue; 

    Assignment(const std::string& lvalue, Expression* rvalue) : Expression(SyntaxType::Assignment), lvalue(lvalue), rvalue(rvalue)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitAssignment(this);
    }
};