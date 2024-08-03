#include "parser.hpp"

const Token Parser::PeekToken()
{
    if (m_Position >= m_Tokens.size())
        return Token(TokenKind::None, 0, 0); 
    return m_Tokens[m_Position];
}

const Token Parser::NextToken()
{
    if (m_Position >= m_Tokens.size())
        return Token(TokenKind::None, 0, 0); 
    const auto& token = m_Tokens[m_Position];
    m_Position++;
    return token; 
}

void Parser::RevertToken()
{
    if (m_Position <= 0) return;
    m_Position--;
}

void Parser::ConsumeToken()
{
    if (m_Position >= m_Tokens.size()) return; 
    m_Position++; 
}

void Parser::ExceptParse(const std::string& msg, const Token& current_token) const
{
    std::string prefix = std::to_string(current_token.line) + ":" + std::to_string(current_token.position) + ": ";
    std::string unexpected = prefix + "Unexpected token: " + TOKEN_KIND_NAMES[current_token.kind];
    if (current_token.kind == TokenKind::Keyword)
        unexpected = prefix + "Unexpected keyword: " + current_token.value;
    throw std::runtime_error(unexpected + "\n" + prefix + msg);
}

void Parser::colon()
{
    colon(NextToken());
}

void Parser::colon(const Token& token) const
{
    if (token.kind != TokenKind::Colon) ExceptParse("error: Expected ':'", token);
}

void Parser::semicolon()
{
    semicolon(NextToken()); 
}

void Parser::semicolon(const Token& token) const
{
    if (token.kind != TokenKind::Semicolon) ExceptParse("error: Expected ';'", token);
}

void Parser::lbrace()
{
    lbrace(NextToken()); 
}

void Parser::lbrace(const Token& token) const
{
    if (token.kind != TokenKind::LeftBrace) ExceptParse("error: Expected '{'", token);
}

void Parser::rbrace()
{
    rbrace(NextToken());
}

void Parser::rbrace(const Token& token) const
{  
    if (token.kind != TokenKind::RightBrace) ExceptParse("error: Expected '}'", token);
}

void Parser::lparen()
{
    lparen(NextToken());
}

void Parser::lparen(const Token& token) const
{
    if (token.kind != TokenKind::LeftParenthesis) ExceptParse("error: Expected '('", token);
}

void Parser::rparen()
{
    rparen(NextToken());
}

void Parser::rparen(const Token& token) const
{
    if (token.kind != TokenKind::RightParenthesis) ExceptParse("error: Expected ')'", token);
}