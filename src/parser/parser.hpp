#pragma once

#include <exception>
#include <memory>
#include <vector>

#include "lexer/lexer.hpp"

#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/program.hpp"
#include "syntax/expr.hpp"

class ParseException : public std::exception
{
    public:
        ParseException(const std::string& msg) : m_Msg(msg)
        {
        }

        virtual const char* what() const throw()
        {
            return m_Msg.c_str();
        }

    private:
        std::string m_Msg; 
};

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

        const Token* PeekToken()
        {
            if (m_Position >= m_Tokens.size())
                return new Token(TokenKind::None); 
            return &m_Tokens[m_Position];
        }

        const Token* NextToken()
        {
            if (m_Position >= m_Tokens.size())
                return new Token(TokenKind::None); 
            const auto& token = m_Tokens[m_Position];
            m_Position++;
            return &token; 
        }

        void ConsumeToken()
        {
            if (m_Position >= m_Tokens.size()) return; 
            m_Position++; 
        }

    private:
        size_t m_Position = 0; 

};