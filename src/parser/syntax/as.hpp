#pragma once

#include "visitor.hpp"

struct AbstractSyntax
{
    virtual void Accept(ASTVisitor* visitor) = 0;
};