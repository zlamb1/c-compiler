#pragma once

#include <vector>

#include "parser/syntax/as.hpp"
#include "parser/syntax/expression/expression.hpp"

struct Statement : public AbstractSyntax
{
    Statement(SyntaxType type) : AbstractSyntax(type) 
    {
    }

    virtual ~Statement() = default; 

    typedef std::shared_ptr<Statement> Ref;
};