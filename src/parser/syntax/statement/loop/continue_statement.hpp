#pragma once

#include "parser/syntax/statement/statement.hpp"

struct ContinueStatement : public Statement
{
    ContinueStatement() : Statement(SyntaxType::Continue)
    {
    }

    typedef std::shared_ptr<ContinueStatement> Ref; 
};