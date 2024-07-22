#pragma once

#include "function.hpp"

struct Program : public AbstractSyntax
{
    Function* function;

    Program(Function* function) : AbstractSyntax(SyntaxType::Program), function(function)
    {
    }
};