#pragma once

#include <memory>

enum class SyntaxType : int
{
    Program,
    Function,
    // Statements
    StatementExpression,
    AssignmentOp,
    Declaration,
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
private:
    SyntaxType _type; 
};

template<typename T, typename ... Args>
constexpr std::shared_ptr<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}