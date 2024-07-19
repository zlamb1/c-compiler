#include "lexer.hpp"

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

            if (str.hasCapacity(2) && str.peek(2) == "0x")
            {
                token.kind = TokenKind::HexConstant;
                token.value += str.pop(2);
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
                case '-':
                    token.kind = TokenKind::Negation;
                    break;
                case '~':
                    token.kind = TokenKind::BWComplement;
                    break; 
                case '!':
                    token.kind = TokenKind::LogicalNegation; 
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
                case TokenKind::HexConstant:
                    if (is_hexdigit(next_char))
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
                case '-':
                case '~':
                case '!':
                    return token; 
            }
        }

        str.pop();
    }
    return token; 
}