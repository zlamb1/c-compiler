#pragma once

#include "function.hpp"

struct Program : public AbstractSyntax
{
    Function* function;

    void accept(ASTVisitor* visitor) override
    {
        visitor->visitProgram(this); 
    }
};