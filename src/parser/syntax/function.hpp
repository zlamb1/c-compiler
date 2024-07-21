#pragma once

#include <string>

#include "statement.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    Statement* statement = nullptr; 

    Function(const std::string& name, Statement* statement) : AbstractSyntax(SyntaxType::Function), name(name), statement(statement)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitFunction(this); 
    }
};