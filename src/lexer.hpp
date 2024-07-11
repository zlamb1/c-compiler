#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "token.hpp"

class Lexer
{
    public:
        std::vector<Token> LexFile(const std::string& filepath); 
        std::vector<Token> LexLines(const std::vector<std::string>& lines);

    private:
        void LexString(const std::string& str, std::vector<Token>& tokens); 
        void FlushToken(Token& token, std::vector<Token>& tokens); 

};