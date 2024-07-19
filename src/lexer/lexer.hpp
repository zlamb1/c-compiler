#pragma once

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "token.hpp"
#include "stack_str.hpp"
#include "utility.hpp"

class Lexer
{
    public:
        Lexer() = default; 
        virtual ~Lexer() = default; 

        virtual const std::vector<Token>& LexFile(const std::string& filepath); 
        virtual Token LexToken(StackString& str); 

    private:
        std::vector<Token> m_Tokens; 
};