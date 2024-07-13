#include "lexer.hpp"

#include <ctype.h>
#include <sstream>
#include <unordered_map>

static inline bool is_digit(char c)
{
    switch (c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true; 
    }

    return false; 
}

static inline bool is_nondigit(char c)
{
    return isalpha(c) || c == '_'; 
}

static inline Token check_keyword(Token& token)
{
    if (token.value == "int" || token.value == "return")
        token.kind = TokenKind::Keyword; 
    return token; 
}

const std::vector<Token>& Lexer::LexFile(const std::string& filepath)
{
    std::ifstream lexfile;
    lexfile.open(filepath);

    if (lexfile.is_open())
    {
        std::stringstream buffer;
        buffer << lexfile.rdbuf();
        const std::string& file_content = buffer.str();
        std::string_view view{ file_content}; 
        StackString str{ view }; 

        while (str.hasCapacity())
        {
            auto token = LexToken(str); 
            if (token.kind != TokenKind::None)
                m_Tokens.emplace_back(token); 
        }

    } else std::cout << "Could not open lex file @ " << filepath << "!" << std::endl;

    return m_Tokens; 
}

Token Lexer::LexToken(StackString& str)
{
    Token token{ TokenKind::None };
    while (str.hasCapacity())
    {
        char next_char = str.peek();

        if (token.kind == TokenKind::None)
        {
            if (is_nondigit(next_char))
            {
                token.kind = TokenKind::Identifier;
                continue;
            }

            if (is_digit(next_char))
            {
                token.kind = TokenKind::IntConstant;
                continue;
            }

            switch (next_char)
            {
                case '(':
                    token.kind = TokenKind::LeftParenthesis; 
                    break;
                case ')':
                    token.kind = TokenKind::RightParenthesis; 
                    break;
                case '{':
                    token.kind = TokenKind::LeftBrace;
                    break;
                case '}':
                    token.kind = TokenKind::RightBrace;
                    break;
                case ';':
                    token.kind = TokenKind::Semicolon;
                    break;
            }

            if (token.kind != TokenKind::None)
            {
                str.pop(); 
                return token;
            }
        }
        else
        {
            switch (token.kind)
            {
                case TokenKind::Identifier:
                    if (is_nondigit(next_char) || is_digit(next_char))
                        token.value += next_char;
                    else return check_keyword(token);
                    break; 
                case TokenKind::IntConstant:
                    if (is_digit(next_char))
                        token.value += next_char;
                    else return token; 
                    break;
            }

            switch (next_char)
            {
                case '(': 
                case ')':
                case '{':
                case '}':
                case ';':
                    return token; 
            }
        }

        str.pop();
    }
    return token; 
}