#pragma once

#include <vector>

#include "compound_block.hpp"

struct Conditional
{
    Expression::Ref condition; 
    Statement::Ref statement; 

    Conditional(Expression::Ref condition, Statement::Ref statement) : condition(condition), statement(statement)
    {
    }
};

struct IfStatement : public Statement
{
    Conditional if_conditional; 
    std::vector<Conditional> else_ifs; 
    Statement::Ref else_statement = nullptr;

    IfStatement(const Conditional& conditional) : Statement(SyntaxType::IfStatement), if_conditional(conditional)
    {
    }

    typedef std::shared_ptr<IfStatement> Ref;  
};