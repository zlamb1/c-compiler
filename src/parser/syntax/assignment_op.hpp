#pragma once

#include "expr.hpp"

enum class AssignmentOpType : int
{
    Add,
    Minus,
    Multiplication,
    Division,
    Modulo,
    LeftShift,
    RightShift,
    LogicalOr,
    LogicalAnd,
    LogicalXOR
};

class AssignmentOp : public Expression
{
public:
    std::string lvalue; 
    Expression* rvalue; 

    AssignmentOp(AssignmentOpType opType, const std::string& lvalue, Expression* rvalue) : 
        Expression(SyntaxType::AssignmentOp), opType(opType), lvalue(lvalue), rvalue(rvalue)
    {
    }

    AssignmentOpType OpType()
    {
        return opType; 
    }
private:
    AssignmentOpType opType; 
};