#pragma once

#include "expression.hpp"

struct NullExpression : public Expression
{
    NullExpression() : Expression(SyntaxType::Null)
    {
    }
};