#pragma once

#include <string>

#include "statement.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    Statement* statement = nullptr; 

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitFunction(this); 
    }
};