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

    void Accept(SyntaxVisitor* syntax_visitor) override
    {
        syntax_visitor->VisitRegisterArg(this);
    }
};

struct ImmediateArg : public AssemblyArg
{
    ImmediateArg(int value) : value(value)
    {
    }

    int value;

    void Accept(SyntaxVisitor* syntax_visitor) override
    {
        syntax_visitor->VisitImmediateArg(this);
    }
};