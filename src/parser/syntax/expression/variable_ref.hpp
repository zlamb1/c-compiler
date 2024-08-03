#pragma once

#include "expression.hpp"

struct VariableRef : public Expression
{
    std::string name; 

    VariableRef(const std::string& name) : Expression(SyntaxType::VariableRef), name(name)
    {
    }

    typedef std::shared_ptr<VariableRef> Ref;
};