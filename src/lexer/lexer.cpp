#include "lexer.hpp"

static inline Token check_keyword(Token& token)
{
    if (token.value == "int" || token.value == "if" || token.value == "else" || token.value == "return")
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
            if (token.kind != TokenKind::None && token.kind != TokenKind::Comment && token.kind != TokenKind::MultilineComment)
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

            if (str.hasCapacity(3))
            {
                auto charSequence = str.peek(3); 
                if (charSequence == "<<=")
                {
                    str.pop(3);
                    return Token(TokenKind::LeftShiftEquals); 
                } else if (charSequence == ">>=")
                {
                    str.pop(3);
                    return Token(TokenKind::RightShiftEquals); 
                }
            }

            if (str.hasCapacity(2))
            {
                auto charSequence = str.peek(2); 
                if (charSequence == "0x")
                {
                    token.kind = TokenKind::HexConstant;
                    token.value += str.pop(2);
                    continue;
                } else if (charSequence == "//")
                {
                    str.pop(2); 
                    token.kind = TokenKind::Comment; 
                    continue;
                } else if (charSequence == "/*")
                {
                    str.pop(2); 
                    token.kind = TokenKind::MultilineComment; 
                    continue; 
                } else if (charSequence == "&&")
                {
                    str.pop(2); 
                    return Token(TokenKind::DoubleAmpersand);
                } else if (charSequence == "||")
                {
                    str.pop(2); 
                    return Token(TokenKind::DoublePipe); 
                } else if (charSequence == "==")
                {
                    str.pop(2);
                    return Token(TokenKind::DoubleEquals); 
                } else if (charSequence == "!=")
                {
                    str.pop(2);
                    return Token(TokenKind::NotEqual); 
                } else if (charSequence == "<=")
                {
                    str.pop(2);
                    return Token(TokenKind::LessThanOrEqual);
                } else if (charSequence == ">=")
                {
                    str.pop(2); 
                    return Token(TokenKind::GreaterThanOrEqual); 
                } else if (charSequence == "<<")
                {
                    str.pop(2);
                    return Token(TokenKind::LeftShift);
                } else if (charSequence == ">>")
                {
                    str.pop(2);
                    return Token(TokenKind::RightShift); 
                } else if (charSequence == "+=")
                {
                    str.pop(2);
                    return Token(TokenKind::AddEquals);
                } else if (charSequence == "-=")
                {
                    str.pop(2);
                    return Token(TokenKind::MinusEquals);
                } else if (charSequence == "*=")
                {
                    str.pop(2);
                    return Token(TokenKind::AsteriskEquals);
                } else if (charSequence == "/=")
                {
                    str.pop(2);
                    return Token(TokenKind::SlashEquals);
                } else if (charSequence == "%=")
                {
                    str.pop(2);
                    return Token(TokenKind::PercentEquals);
                } else if (charSequence == "<<=")
                {
                    str.pop(2);
                    return Token(TokenKind::LeftShiftEquals);
                } else if (charSequence == ">>=")
                {
                    str.pop(2);
                    return Token(TokenKind::RightShiftEquals);
                } else if (charSequence == "|=")
                {
                    str.pop(2);
                    return Token(TokenKind::OrEquals);
                } else if (charSequence == "&=")
                {
                    str.pop(2); 
                    return Token(TokenKind::AndEquals);
                } else if (charSequence == "^=")
                {
                    str.pop(2);
                    return Token(TokenKind::CaretEquals);
                }
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
                    token.kind = TokenKind::Minus;
                    break;
                case '~':
                    token.kind = TokenKind::Tilde;
                    break; 
                case '!':
                    token.kind = TokenKind::Exclamation; 
                    break;
                case '+':
                    token.kind = TokenKind::Plus;
                    break; 
                case '*':
                    token.kind = TokenKind::Asterisk;
                    break;
                case '/':
                    token.kind = TokenKind::Slash;
                    break; 
                case '&':
                    token.kind = TokenKind::Ampersand;
                    break;
                case '|':
                    token.kind = TokenKind::Pipe;
                    break;
                case '<':
                    token.kind = TokenKind::LessThan;
                    break;
                case '>':
                    token.kind = TokenKind::GreaterThan;
                    break;
                case '%':
                    token.kind = TokenKind::Percent;
                    break; 
                case '^':
                    token.kind = TokenKind::Caret;
                    break; 
                case '=':
                    token.kind = TokenKind::Equal;
                    break;
                case ',':
                    token.kind = TokenKind::Comma;
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
                case TokenKind::Comment:
                    if (next_char == '\n')
                    {
                        std::cout << "Comment: " << token.value << std::endl;
                        str.pop(); 
                        return token; 
                    } 
                    token.value += str.pop(); 
                    continue;
                case TokenKind::MultilineComment:
                    if (str.hasCapacity(2) && str.peek(2) == "*/")
                    {
                        str.pop(2);
                        return token; 
                    }
                    token.value += str.pop(); 
                    continue;
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
                case '+':
                case '*':
                case '/':
                case '&':
                case '|':
                case '=':
                case '<':
                case '>':
                case '%':
                case '^': 
                case ',':
                    return token; 
            }
        }

        str.pop();
    }
    return token; 
}