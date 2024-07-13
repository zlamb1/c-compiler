#include "lexer.hpp"

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
            m_Tokens.emplace_back(token); 
        }

    } else std::cout << "Could not open lex file @ " << filepath << "!" << std::endl;

    return m_Tokens; 
}

Token Lexer::LexToken(StackString& str)
{
    str.pop(); 
    return {TokenKind::Identifier}; 
}