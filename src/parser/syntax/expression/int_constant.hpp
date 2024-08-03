#pragma once

#include "expression.hpp"

struct IntConstant : public Expression
{
    int value;

    IntConstant(int value) : Expression(SyntaxType::IntConstant), value(value)
    {
    }

    typedef std::shared_ptr<IntConstant> Ref;
};