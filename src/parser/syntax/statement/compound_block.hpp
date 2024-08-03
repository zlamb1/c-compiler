#pragma once

#include <vector>

#include "statement.hpp"

struct CompoundBlock : public Statement
{
    std::vector<Statement::Ref> statements; 

    CompoundBlock() : Statement(SyntaxType::CompoundBlock)
    {
    } 

    CompoundBlock(Statement::Ref statement) : Statement(SyntaxType::CompoundBlock)
    {
        statements.emplace_back(statement); 
    }

    typedef std::shared_ptr<CompoundBlock> Ref; 
};