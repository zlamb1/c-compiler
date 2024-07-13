#pragma once

#include "visitor.hpp"

struct AbstractSyntax
{
    virtual void accept(ASTVisitor* visitor) = 0;
};