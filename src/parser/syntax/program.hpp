#pragma once

#include "function.hpp"

struct Program : public AbstractSyntax
{
    Function* function;

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitProgram(this); 
    }
};