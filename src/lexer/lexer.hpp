#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "token.hpp"
#include "stack_str.hpp"

class Lexer
{
    public:
        const std::vector<Token>& LexFile(const std::string& filepath); 
        Token LexToken(StackString& str); 

    private:
        std::vector<Token> m_Tokens; 
};