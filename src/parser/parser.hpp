#pragma once

#include <vector>

#include "lexer/token.hpp"

#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/program.hpp"
#include "syntax/expr.hpp"

class Parser
{
    public:
        Parser(std::vector<Token>& tokens) : m_Tokens(tokens) 
        {
        }

        virtual AbstractSyntax* parse() = 0; 

    protected:
        const Token* nextToken()
        {
            if (m_Position >= m_Tokens.size())
            {
                auto token = new Token(TokenKind::None); 
                return token; 
            }
            const auto& token = m_Tokens[m_Position];
            m_Position++;
            return &token; 
        }

    private:
        std::vector<Token> m_Tokens;
        size_t m_Position = 0; 

};