#pragma once

#include <memory>
#include <vector>

#include "lexer/lexer.hpp"

#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/program.hpp"
#include "syntax/expr.hpp"

class Parser
{
    public:
        Parser(std::unique_ptr<Lexer> lexer) : m_Lexer(std::move(lexer))
        {
        }
         
        virtual ~Parser() = default; 

        virtual AbstractSyntax* ParseFile(const std::string& filepath) = 0; 
        virtual void PrintTree(AbstractSyntax* ast) = 0; 

    protected:
        std::unique_ptr<Lexer> m_Lexer; 
        std::vector<Token> m_Tokens; 

        const Token* NextToken()
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
        size_t m_Position = 0; 

};