#pragma once

#include <memory>

#include "utility/ref.hpp"

enum class SyntaxType : int
{
    Program,
    Function,
    // Statements
    StatementExpression,
    AssignmentOp,
    Declaration,
    CompoundBlock,
    IfStatement,
    Return,
    // Expressions
    UnaryOp,
    BinaryOp,
    IntConstant,
    VariableRef,
    Assignment
};

class AbstractSyntax
{
public:
    AbstractSyntax(SyntaxType type) : _type(type) 
    {
    }
    
    virtual ~AbstractSyntax() = default;

    SyntaxType type()
    {
        return _type; 
    }

    typedef std::shared_ptr<AbstractSyntax> Ref; 
    template<typename T>
    static std::shared_ptr<T> RefCast(Ref ref)
    {
        return std::dynamic_pointer_cast<T>(ref); 
    }
private:
    SyntaxType _type; 
};