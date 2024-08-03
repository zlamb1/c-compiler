#pragma once

#include "parser/syntax/statement/statement.hpp"

struct BreakStatement : public Statement
{
    BreakStatement() : Statement(SyntaxType::Break)
    {
    }

    typedef std::shared_ptr<BreakStatement> Ref; 
};