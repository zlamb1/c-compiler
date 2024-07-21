#pragma once

#include "int_conv.hpp"
#include "parser.hpp"
#include "print.hpp"

static std::unordered_map<TokenKind, UnaryOpType> TOKEN_TO_UNARY_TYPE
{
    { TokenKind::Minus, UnaryOpType::Negation },
    { TokenKind::Tilde, UnaryOpType::Complement },
    { TokenKind::Exclamation, UnaryOpType::LogicalNegation }
};

static std::unordered_map<TokenKind, BinaryOpType> TOKEN_TO_BINARY_TYPE
{
    { TokenKind::Plus, BinaryOpType::Addition },
    { TokenKind::Minus, BinaryOpType::Subtraction },
    { TokenKind::Asterisk, BinaryOpType::Multiplication },
    { TokenKind::Slash, BinaryOpType::Division },
    { TokenKind::Pipe, BinaryOpType::BitwiseOr },
    { TokenKind::DoublePipe, BinaryOpType::LogicalOr },
    { TokenKind::Ampersand, BinaryOpType::BitwiseAnd },
    { TokenKind::DoubleAmpersand, BinaryOpType::LogicalAnd },
    { TokenKind::DoubleEquals, BinaryOpType::Equal },
    { TokenKind::NotEqual, BinaryOpType::NotEqual },
    { TokenKind::LessThan, BinaryOpType::LessThan },
    { TokenKind::LessThanOrEqual, BinaryOpType::LessThanOrEqual },
    { TokenKind::GreaterThan, BinaryOpType::GreaterThan },
    { TokenKind::GreaterThanOrEqual, BinaryOpType::GreaterThanOrEqual },
    { TokenKind::Percent, BinaryOpType::Remainder },
    { TokenKind::Caret, BinaryOpType::BitwiseXOR },
    { TokenKind::LeftShift, BinaryOpType::BitwiseLeftShift },
    { TokenKind::RightShift, BinaryOpType::BitwiseRightShift }
};

// Recursive-Descent Parser
class RDParser : public Parser
{
    public:
        RDParser(std::unique_ptr<Lexer> lexer) : Parser(std::move(lexer))
        {
        }

        AbstractSyntax* ParseFile(const std::string& filepath) override
        {
            m_Tokens = m_Lexer->LexFile(filepath); 
            std::cout << "<--- Lexical Analysis --->" << std::endl;
            for (auto& token : m_Tokens)
                std::cout << TOKEN_KIND_NAMES[token.kind] << ": " << (!token.value.empty() ? token.value : "NULL") << std::endl;

            return ParseProgram(); 
        }

        void PrintTree(AbstractSyntax* ast) override
        {
            if (ast)
            {
                std::cout << std::endl;
                std::cout << "<--- Abstract Syntax Tree --->" << std::endl;
                pretty_print_ast(ast);
            }
        }

    private:
        void ExceptParse(const std::string& msg)
        {
            throw ParseException(msg); 
        }

        Program* ParseProgram()
        {
            Program* p = new Program(ParseFunction()); 
            return p; 
        }

        Function* ParseFunction()
        {
            auto token = NextToken();
            if (token->kind != TokenKind::Keyword || token->value != "int") ExceptParse("Invalid Return Type"); 
            token = NextToken(); 
            if (token->kind != TokenKind::Identifier) ExceptParse("Invalid Function Name"); 
            std::string name = token->value; 
            token = NextToken(); 
            if (token->kind != TokenKind::LeftParenthesis) ExceptParse("Malformed Function Definition"); 
            token = NextToken(); 
            if (token->kind != TokenKind::RightParenthesis) ExceptParse("Malformed Function Definition"); 
            token = NextToken(); 
            if (token->kind != TokenKind::LeftBrace) ExceptParse("Expected Block: expected '{'");
            auto statement = ParseStatement(); 
            token = NextToken(); 
            if (token->kind != TokenKind::RightBrace) ExceptParse("Unclosed Block: expected '}'");
            return new Function(name, statement);
        }

        Statement* ParseStatement()
        {
            auto token = NextToken(); 
            if (token->kind != TokenKind::Keyword || token->value != "return") ExceptParse("Invalid Statement"); 
            auto expr = ParseExpression(); 
            token = NextToken(); 
            if (token->kind != TokenKind::Semicolon) ExceptParse("Unclosed Statement: expected ';'");
            return new Return(expr); 
        }
        
        Expression* ParseExpression()
        {
            auto expr = ParseLogicalAndExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::DoublePipe)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseLogicalAndExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseLogicalAndExpression()
        {
            auto expr = ParseBitwiseOrExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::DoubleAmpersand)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseBitwiseOrExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseBitwiseOrExpression()
        {
            auto expr = ParseBitwiseXORExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::Pipe)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseBitwiseXORExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseBitwiseXORExpression()
        {
            auto expr = ParseBitwiseAndExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::Caret)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseBitwiseAndExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseBitwiseAndExpression()
        {
            auto expr = ParseEqualityExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::Ampersand)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseEqualityExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseEqualityExpression()
        {
            auto expr = ParseRelationalExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::DoubleEquals || token->kind == TokenKind::NotEqual)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseRelationalExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseRelationalExpression()
        {
            auto expr = ParseShiftExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression");
            while (token->kind == TokenKind::LessThan || token->kind == TokenKind::LessThanOrEqual ||
                   token->kind == TokenKind::GreaterThan || token->kind == TokenKind::GreaterThanOrEqual)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseShiftExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseShiftExpression()
        {
            auto expr = ParseAdditiveExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression");
            while (token->kind == TokenKind::LeftShift || token->kind == TokenKind::RightShift)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind];
                auto nextExpr = ParseAdditiveExpression(); 
                expr = new BinaryOp(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseAdditiveExpression()
        {
            auto term = ParseTerm(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression");
            while (token->kind == TokenKind::Plus || token->kind == TokenKind::Minus)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind]; 
                auto nextTerm = ParseTerm(); 
                term = new BinaryOp(type, term, nextTerm); 
                token = PeekToken(); 
            }
            return term; 
        }

        Expression* ParseTerm()
        {
            auto factor = ParseFactor(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Term"); 
            while (token->kind == TokenKind::Asterisk || token->kind == TokenKind::Slash || token->kind == TokenKind::Percent)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind]; 
                auto nextFactor = ParseFactor(); 
                factor = new BinaryOp(type, factor, nextFactor); 
                token = PeekToken(); 
            }
            return factor; 
        }

        Expression* ParseFactor()
        {
            auto token = NextToken();
            if (token->kind == TokenKind::LeftParenthesis)
            {
                auto expr = ParseExpression(); 
                token = NextToken();
                if (token->kind != TokenKind::RightParenthesis) ExceptParse("Unclosed Expression: expected ')'"); 
                return expr; 
            } else if (TOKEN_TO_UNARY_TYPE.find(token->kind) != TOKEN_TO_UNARY_TYPE.end())
            {
                auto factor = ParseFactor(); 
                return new UnaryOp(TOKEN_TO_UNARY_TYPE[token->kind], factor); 
            } else if (token->kind == TokenKind::IntConstant || token->kind == TokenKind::HexConstant)
            {
                return new IntConstant(parse_c_int(token->value)); 
            } else ExceptParse("Invalid Factor: " + TOKEN_KIND_NAMES[token->kind]); 
            return nullptr; 
        }
};