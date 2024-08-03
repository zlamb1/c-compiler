#pragma once

#include <string>
#include <vector>

#include "statement/statement.hpp"
#include "statement/compound_block.hpp"

struct Function : public AbstractSyntax
{
    std::string name; 
    CompoundBlock::Ref block; 

    Function(const std::string& name, CompoundBlock::Ref block) : AbstractSyntax(SyntaxType::Function), name(name), block(block)
    {
    }

    const std::vector<Statement::Ref> Statements() const
    {
        return block->statements;
    }

    typedef std::shared_ptr<Function> Ref;
};