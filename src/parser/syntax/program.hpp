#pragma once

#include "function.hpp"

struct Program : public AbstractSyntax
{
    Function* function;

    Program(Function* function) : AbstractSyntax(SyntaxType::Program), function(function)
    {
    }

    void Accept(ASTVisitor* visitor) override
    {
        visitor->VisitProgram(this); 
    }
};