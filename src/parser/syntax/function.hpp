#pragma once

#include <string>

#include "statement.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    Statement* statement = nullptr; 

    void accept(ASTVisitor* visitor) override
    {
        visitor->visitFunction(this); 
    }
};