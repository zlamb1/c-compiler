#pragma once

#include <vector>

#include "as.hpp"
#include "expr.hpp"

struct Statement : public AbstractSyntax
{
    Statement(SyntaxType type) : AbstractSyntax(type) 
    {
    }

    virtual ~Statement() = default; 

    typedef std::shared_ptr<Statement> Ref;
};