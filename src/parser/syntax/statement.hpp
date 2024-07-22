#pragma once

#include <vector>

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

struct Variable
{
    std::string name; 
    Expression* expr; 

    Variable(const std::string& name, Expression* expr) : name(name), expr(expr)
    {
    }
};

struct Declaration : public Statement
{
    std::vector<Variable> variables; 

    Declaration() : Statement(SyntaxType::Declaration)
    {
    }

    Declaration(Variable var) : Statement(SyntaxType::Declaration)
    {
        variables.emplace_back(var); 
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