#pragma once

#include <exception>
#include <memory>
#include <ostream>
#include <vector>

#include "lexer/lexer.hpp"

#include "syntax/includes.hpp"

class Parser
{
public:
    Parser(std::unique_ptr<Lexer> lexer) : m_Lexer(std::move(lexer))
    {
    }
        
    virtual ~Parser() = default; 

    virtual AbstractSyntax::Ref ParseFile(const std::string& filepath) = 0; 
    virtual void LogTree(std::ostream& out, const AbstractSyntax::Ref root) const = 0; 

protected:
    std::unique_ptr<Lexer> m_Lexer; 
    std::vector<Token> m_Tokens; 

    const Token PeekToken();
    const Token NextToken();
    void RevertToken();
    void ConsumeToken();

    void ExceptParse(const std::string& msg, const Token& current_token) const;

    void colon();
    void colon(const Token& token) const;
    void semicolon();
    void semicolon(const Token& token) const;
    void lbrace();
    void lbrace(const Token& token) const;
    void rbrace();
    void rbrace(const Token& token) const;
    void lparen();
    void lparen(const Token& token) const;
    void rparen();
    void rparen(const Token& token) const;

private:
    size_t m_Position = 0; 
};