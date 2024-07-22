#pragma once

#include "function.hpp"

struct Program : public AbstractSyntax
{
    Function::Ref function;

    Program(Function::Ref function) : AbstractSyntax(SyntaxType::Program), function(function)
    {
    }

    typedef std::shared_ptr<Program> Ref;
};