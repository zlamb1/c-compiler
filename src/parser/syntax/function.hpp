#pragma once

#include <string>
#include <vector>

#include "statement.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    std::vector<Statement::Ref> statements{}; 

    Function(const std::string& name) : AbstractSyntax(SyntaxType::Function), name(name)
    {
    }

    typedef std::shared_ptr<Function> Ref;
};