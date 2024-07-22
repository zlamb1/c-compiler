#pragma once

#include "statement.hpp"

struct Variable
{
    std::string name; 
    Expression::Ref expr; 

    Variable(const std::string& name, Expression::Ref expr) : name(name), expr(expr)
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

    typedef std::shared_ptr<Declaration> Ref;
};