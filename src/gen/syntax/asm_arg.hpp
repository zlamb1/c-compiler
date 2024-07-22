#pragma once

#include "visitor.hpp"

enum class ArgType : int
{
    Register,
    Offset,
    Immediate,
    Label
};

class AssemblyArg
{
public:
    AssemblyArg(ArgType type) : _type(type)
    {
    }
    virtual ~AssemblyArg() = default; 

    ArgType type() const
    {
        return _type; 
    }

    virtual void Accept(SyntaxVisitor* syntax_visitor) = 0;
private:
    ArgType _type; 
};

struct RegisterArg : public AssemblyArg
{
    RegisterArg(const std::string& _register) : AssemblyArg(ArgType::Register), _register(_register)
    {
    }

    std::string _register; 

    void Accept(SyntaxVisitor* syntaxVisitor) override
    {
        syntaxVisitor->VisitRegisterArg(this);
    }
};

struct OffsetArg : public AssemblyArg
{
    OffsetArg(const std::string& _register, int offset) : AssemblyArg(ArgType::Offset), _register(_register), offset(offset)
    {
    }

    std::string _register; 
    int offset;

    void Accept(SyntaxVisitor* syntaxVisitor)
    {
        syntaxVisitor->VisitOffsetArg(this); 
    }
};

struct ImmediateArg : public AssemblyArg
{
    ImmediateArg(int value) : AssemblyArg(ArgType::Immediate), value(value)
    {
    }

    int value;

    void Accept(SyntaxVisitor* syntaxVisitor) override
    {
        syntaxVisitor->VisitImmediateArg(this);
    }
};

struct LabelArg : public AssemblyArg
{
    LabelArg(const std::string& label) : AssemblyArg(ArgType::Label), label(label)
    {
    }

    std::string label;

    void Accept(SyntaxVisitor* syntaxVisitor) override
    {
        syntaxVisitor->VisitLabelArg(this); 
    }
};