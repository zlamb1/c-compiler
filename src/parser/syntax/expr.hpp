#pragma once

#include "as.hpp"

struct Expression : public AbstractSyntax
{
    Expression(SyntaxType type) : AbstractSyntax(type) 
    {
    }

    virtual ~Expression() = default; 

    typedef std::shared_ptr<Expression> Ref;
};
