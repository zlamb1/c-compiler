#pragma once

#include "statement.hpp"

struct Variable
{
    std::string name; 
    Expression::Ref expression; 

    Variable(const std::string& name, Expression::Ref expression) : name(name), expression(expression)
    {
    }
};

struct Declaration : public Statement
{
    size_t type_size = 0; 
    std::vector<Variable> variables; 

    Declaration(size_t type_size) : Statement(SyntaxType::Declaration), type_size(type_size)
    {
    }

    Declaration(size_t type_size, Variable var) : Statement(SyntaxType::Declaration), type_size(type_size)
    {
        variables.emplace_back(var); 
    }

    typedef std::shared_ptr<Declaration> Ref;
};