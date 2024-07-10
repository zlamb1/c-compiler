#pragma once

#include <string>

#include "token_kind.hpp"

struct Token
{
    TokenKind kind;
    std::string value; 
};