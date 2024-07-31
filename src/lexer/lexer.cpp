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
    Token token{ TokenKind::None, 0, 0 };
    while (str.hasCapacity())
    {
        char next_char = str.peek();
        if (token.kind == TokenKind::None)
        {
            token.line = str.line(); 
            token.position = str.line_position(); 

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

            if (str.hasCapacity(3))
            {
                auto charSequence = str.peek(3); 
                if (charSequence == "<<=")
                {
                    auto token = CreateToken(TokenKind::LeftShiftEquals, str); 
                    str.pop(3);
                    return token; 
                } else if (charSequence == ">>=")
                {
                    auto token = CreateToken(TokenKind::RightShiftEquals, str);
                    str.pop(3);
                    return token;
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
                    auto token = CreateToken(TokenKind::DoubleAmpersand, str);
                    str.pop(2); 
                    return token;
                } else if (charSequence == "||")
                {
                    auto token = CreateToken(TokenKind::DoublePipe, str);
                    str.pop(2); 
                    return token;
                } else if (charSequence == "==")
                {
                    auto token = CreateToken(TokenKind::DoubleEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "!=")
                {
                    auto token = CreateToken(TokenKind::NotEqual, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "<=")
                {
                    auto token = CreateToken(TokenKind::LessThanOrEqual, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == ">=")
                {
                    auto token = CreateToken(TokenKind::GreaterThanOrEqual, str);
                    str.pop(2); 
                    return token;
                } else if (charSequence == "<<")
                {
                    auto token = CreateToken(TokenKind::LeftShift, str);
                    str.pop(2);
                    return token; 
                } else if (charSequence == ">>")
                {
                    auto token = CreateToken(TokenKind::RightShift, str);
                    str.pop(2);
                    return token; 
                } else if (charSequence == "+=")
                {
                    auto token = CreateToken(TokenKind::AddEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "-=")
                {
                    auto token = CreateToken(TokenKind::MinusEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "*=")
                {
                    auto token = CreateToken(TokenKind::AsteriskEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "/=")
                {
                    auto token = CreateToken(TokenKind::SlashEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "%=")
                {
                    auto token = CreateToken(TokenKind::PercentEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "<<=")
                {
                    auto token = CreateToken(TokenKind::LeftShiftEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == ">>=")
                {
                    auto token = CreateToken(TokenKind::RightShiftEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "|=")
                {
                    auto token = CreateToken(TokenKind::OrEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "&=")
                {
                    auto token = CreateToken(TokenKind::AddEquals, str);
                    str.pop(2); 
                    return token;
                } else if (charSequence == "^=")
                {
                    auto token = CreateToken(TokenKind::CaretEquals, str);
                    str.pop(2);
                    return token;
                } else if (charSequence == "++")
                {
                    auto token = CreateToken(TokenKind::PlusPlus, str);
                    str.pop(2);
                    return token; 
                } else if (charSequence == "--")
                {
                    auto token = CreateToken(TokenKind::MinusMinus, str);
                    str.pop(2);
                    return token;
                }
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