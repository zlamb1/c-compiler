#include "lexer.hpp"

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

static inline TokenKind parse_token_kind(const std::string& accum)
{
    if (accum == "true")    return TokenKind::True;
    if (accum == "false")   return TokenKind::False;

    if (accum == "package") return TokenKind::Package;
    if (accum == "struct")  return TokenKind::Struct;
    if (accum == "class")   return TokenKind::Class;
    if (accum == "include") return TokenKind::Include;
    if (accum == "enum")    return TokenKind::Enum;
    if (accum == "fn")      return TokenKind::Fn;

    if (!accum.empty() && accum.find_first_not_of(' ') != std::string::npos)
        return TokenKind::Identifier;

    return TokenKind::None;
}

std::vector<Token> Lexer::LexFile(const std::string& filepath)
{
    std::ifstream lexfile;
    lexfile.open(filepath);

    if (lexfile.is_open())
    {
        std::vector<std::string> lines; 
        std::string line;
        while (std::getline(lexfile, line))
        {
            std::cout << "Read line: " << line << std::endl;
            lines.emplace_back(line); 
        }

        return LexLines(lines); 
    } else
    {
        std::cout << "Could not open lex file @ " << filepath << "!" << std::endl;
    }

    return {}; 
}

std::vector<Token> Lexer::LexLines(const std::vector<std::string>& lines)
{
    std::vector<Token> tokens{}; 
    for (auto& line : lines)
    {
        LexString(line, tokens); 
    }
    return tokens; 
}

void Lexer::LexString(const std::string& str, std::vector<Token>& tokens)
{
    size_t len = str.length(); 
    Token token{};

    for (size_t char_index = 0; char_index < len; char_index++)
    {
        char cur_char = str[char_index];
        char next_char = char_index + 1 < len ? str[char_index + 1] : 0; 

        // We've hit a comment, so we can disregard the rest of the line
        if (cur_char == '#')
        {
            FlushToken(token, tokens);
            break;
        }

        switch (token.kind)
        {                
            case TokenKind::IntegerLiteral:
                if (!is_digit(cur_char))
                {
                    tokens.emplace_back(token); 
                    token = {}; 
                    break;
                } else if (cur_char == '.') {
                    token.kind = TokenKind::DoubleLiteral; 
                    continue; 
                } else {
                    token.value += cur_char; 
                    continue;
                }
            case TokenKind::DoubleLiteral:
                if (is_digit(cur_char))
                {
                    token.value += cur_char;
                    continue;
                } else if (cur_char == 'f' || cur_char == 'F')
                {
                    token.kind = TokenKind::FloatLiteral; 
                    FlushToken(token, tokens);
                    continue; 
                } else
                {
                    FlushToken(token, tokens);
                    break;
                }
            case TokenKind::FloatLiteral:
                if (is_digit(cur_char))
                {
                    token.value += cur_char; 
                    continue;
                } else
                {
                    FlushToken(token, tokens);
                    break; 
                }
        }

        if (token.kind != TokenKind::Identifier && is_digit(cur_char))
        {
            FlushToken(token, tokens);
            token.kind = TokenKind::IntegerLiteral;
            token.value += cur_char; 
            continue;
        }  

        switch (cur_char)
        {
            case '\"':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::Quote });
                continue;
            case '\'':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::SingleQuote });
                continue; 
            case '(':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::LeftParen });
                continue;
            case ')':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::RightParen });
                continue;
            case '[':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::LeftBracket });
                continue;
            case ']':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::RightBracket });
                continue;
            case '{':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::LeftBrace });
                continue;
            case '}':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::RightBrace });
                continue;
            case ';':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::SemiColon });
                continue;
            case ':':
                FlushToken(token, tokens);
                if (next_char == ':')
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ColonColon });
                } else tokens.emplace_back(Token{ TokenKind::Colon });
                continue;
            case '=':
                FlushToken(token, tokens);
                if (next_char == '=') 
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::CompareEqual });
                } else tokens.emplace_back(Token{ TokenKind::Assignment });
                continue;
            case '+':
                FlushToken(token, tokens);
                if (next_char == '+') 
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ArithmeticIncrement });
                } else if (next_char == '=')
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ArtihmeticAddEquals });
                } else tokens.emplace_back(Token{ TokenKind::ArithmeticAdd });
                continue;
            case '-':
                FlushToken(token, tokens);
                if (next_char == '-') 
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ArithmeticDecrement });
                }
                if (next_char == '=') 
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ArithmeticMinusEquals }); 
                } else tokens.emplace_back(Token{ TokenKind::ArithmeticMinus });
                continue;
            case '/':
                FlushToken(token, tokens); 
                if (next_char == '=')
                {
                    char_index++; 
                    tokens.emplace_back(Token{ TokenKind::ArithmeticDivideEquals });
                } else tokens.emplace_back(Token{ TokenKind::ArithmeticDivide });
                continue;
            case '*':
                FlushToken(token, tokens); 
                if (next_char == '=')
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::ArithmeticMultiplyEquals });
                } else tokens.emplace_back(Token{ TokenKind::ArithmeticMultiply });
                continue;
            case '%':
                FlushToken(token, tokens);
                tokens.emplace_back(Token{ TokenKind::ArithmeticModulo });
                continue;
            case '!':
                FlushToken(token, tokens); 
                if (next_char == '=')
                {
                    char_index++; 
                    tokens.emplace_back(Token{ TokenKind::CompareNotEqual }); 
                } else tokens.emplace_back(Token{ TokenKind::CompareNot });
                continue; 
            case '>':
                FlushToken(token, tokens);
                if (next_char == '=')
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::CompareGreaterThanOrEqual });
                } else tokens.emplace_back(Token{ TokenKind::CompareGreaterThan });
                continue;
            case '<':
                FlushToken(token, tokens);
                if (next_char == '=')
                {
                    char_index++;
                    tokens.emplace_back(Token{ TokenKind::CompareLessThanOrEqual });
                } else tokens.emplace_back(Token{ TokenKind::CompareLessThan });
                continue;
        }

        if (token.kind == TokenKind::None && cur_char != ' ')
            token.kind = TokenKind::Identifier; 

        if (token.kind == TokenKind::Identifier)
        {
            if (cur_char == ' ')
            {
                FlushToken(token, tokens);
            } else token.value += cur_char; 
        }
    }
}

void Lexer::FlushToken(Token& token, std::vector<Token>& tokens)
{
    const std::string& accumulator = token.value; 
    token.kind = parse_token_kind(accumulator); 
    if (token.kind != TokenKind::None)
    {
        if (token.kind != TokenKind::Identifier) token.value = ""; 
        tokens.emplace_back(token); 
    }
    token = {}; 
}