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

struct StatementExpression : public Statement
{
    StatementExpression(Expression* expr) : Statement(SyntaxType::StatementExpression), expr(expr)
    {
    }

    Expression* expr; 

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitStatementExpression(this); 
    }
};

struct Declaration : public Statement
{
    std::string name;
    Expression* expr;

    Declaration(const std::string& name) : Statement(SyntaxType::Declaration), name(name), expr(nullptr)
    {
    }

    Declaration(const std::string& name, Expression* expr) : Statement(SyntaxType::Declaration), name(name), expr(expr)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitDeclaration(this); 
    }
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