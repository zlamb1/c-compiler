#pragma once

#include <string>
#include <vector>

#include "statement.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    std::vector<Statement*> statements{}; 

    Function(const std::string& name) : AbstractSyntax(SyntaxType::Function), name(name)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitFunction(this); 
    }
};