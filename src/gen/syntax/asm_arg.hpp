#pragma once

#include "visitor.hpp"

struct AssemblyArg
{
    virtual void Accept(SyntaxVisitor* syntax_visitor) = 0;
};

struct RegisterArg : public AssemblyArg
{
    RegisterArg(const std::string& _register) : _register(_register)
    {
    }

    std::string _register; 

    void Accept(SyntaxVisitor* syntaxVisitor) override
    {
        syntaxVisitor->VisitRegisterArg(this);
    }
};

struct ImmediateArg : public AssemblyArg
{
    ImmediateArg(int value) : value(value)
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
    LabelArg(const std::string& label) : label(label)
    {
    }

    std::string label;

    void Accept(SyntaxVisitor* syntaxVisitor) override
    {
        syntaxVisitor->VisitLabelArg(this); 
    }
};