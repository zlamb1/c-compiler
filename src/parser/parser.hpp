#pragma once

#include <exception>
#include <memory>
#include <vector>

#include "lexer/lexer.hpp"

#include "syntax/includes.hpp"

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

    virtual AbstractSyntax::Ref ParseFile(const std::string& filepath) = 0; 
    virtual void PrintTree(AbstractSyntax::Ref ast) = 0; 

protected:
    std::unique_ptr<Lexer> m_Lexer; 
    std::vector<Token> m_Tokens; 

    const Token PeekToken()
    {
        if (m_Position >= m_Tokens.size())
            return Token(TokenKind::None, 0, 0); 
        return m_Tokens[m_Position];
    }

    const Token NextToken()
    {
        if (m_Position >= m_Tokens.size())
            return Token(TokenKind::None, 0, 0); 
        const auto& token = m_Tokens[m_Position];
        m_Position++;
        return token; 
    }

    void RevertToken()
    {
        if (m_Position <= 0) return;
        m_Position--;
    }

    void ConsumeToken()
    {
        if (m_Position >= m_Tokens.size()) return; 
        m_Position++; 
    }

    void ExceptParse(const std::string& msg, const Token& current_token) const
    {
        std::string prefix = std::to_string(current_token.line) + ":" + std::to_string(current_token.position) + ": ";
        std::string unexpected = prefix + "Unexpected token: " + TOKEN_KIND_NAMES[current_token.kind];
        if (current_token.kind == TokenKind::Keyword)
            unexpected = prefix + "Unexpected keyword: " + current_token.value;
        throw ParseException(unexpected + "\n" + prefix + msg); 
    }

    void colon()
    {
        colon(NextToken());
    }

    void colon(const Token& token) const
    {
        if (token.kind != TokenKind::Colon) ExceptParse("error: Expected ':'", token);
    }

    void semicolon()
    {
        semicolon(NextToken()); 
    }

    void semicolon(const Token& token) const
    {
        if (token.kind != TokenKind::Semicolon) ExceptParse("error: Expected ';'", token);
    }

    void lbrace()
    {
        lbrace(NextToken()); 
    }

    void lbrace(const Token& token) const
    {
        if (token.kind != TokenKind::LeftBrace) ExceptParse("error: Expected '{'", token);
    }

    void rbrace()
    {
        rbrace(NextToken());
    }

    void rbrace(const Token& token) const
    {  
        if (token.kind != TokenKind::RightBrace) ExceptParse("error: Expected '}'", token);
    }

    void lparen()
    {
        lparen(NextToken());
    }

    void lparen(const Token& token) const
    {
        if (token.kind != TokenKind::LeftParenthesis) ExceptParse("error: Expected '('", token);
    }

    void rparen()
    {
        rparen(NextToken());
    }

    void rparen(const Token& token) const
    {
        if (token.kind != TokenKind::RightParenthesis) ExceptParse("error: Expected ')'", token);
    }

private:
    size_t m_Position = 0; 
};