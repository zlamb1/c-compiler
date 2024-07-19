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
    { TokenKind::Slash, BinaryOpType::Division }
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
            /*std::cout << "<--- Lexical Analysis --->" << std::endl;
            for (auto& token : m_Tokens)
                std::cout << TOKEN_KIND_NAMES[token.kind] << ": " << (token.value.empty() ? token.value : "NULL") << std::endl;*/

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
            auto function = ParseFunction(); 
            Program* p = new Program; 
            p->function = function; 
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
            Function* func = new Function;
            func->name = name; 
            func->statement = statement; 
            return func; 
        }

        Statement* ParseStatement()
        {
            auto token = NextToken(); 
            if (token->kind != TokenKind::Keyword || token->value != "return") ExceptParse("Invalid Statement"); 
            auto expr = ParseExpression(); 
            token = NextToken(); 
            if (token->kind != TokenKind::Semicolon) ExceptParse("Unclosed Statement: expected ';'");
            auto ret = new Return; 
            ret->expr = expr; 
            return ret; 
        }

        Expression* ParseExpression()
        {
            auto term = ParseTerm(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression");
            while (token->kind == TokenKind::Plus || token->kind == TokenKind::Minus)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind]; 
                auto nextTerm = ParseTerm(); 
                auto op = new BinaryOp; 
                op->type = type; 
                op->lvalue = term;
                op->rvalue = nextTerm;
                term = op; 
                token = PeekToken(); 
            }
            return term; 
        }

        Expression* ParseTerm()
        {
            auto factor = ParseFactor(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Term"); 
            while (token->kind == TokenKind::Asterisk || token->kind == TokenKind::Slash)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token->kind]; 
                auto nextFactor = ParseFactor(); 
                auto op = new BinaryOp; 
                op->type = type; 
                op->lvalue = factor;
                op->rvalue = nextFactor;
                factor = op; 
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
                auto op = new UnaryOp;
                op->type = TOKEN_TO_UNARY_TYPE[token->kind];
                op->expr = factor; 
                return op; 
            } else if (token->kind == TokenKind::IntConstant || token->kind == TokenKind::HexConstant)
            {
                auto expr = new IntExpr; 
                expr->value = parse_c_int(token->value); 
                return expr; 
            } else ExceptParse("Invalid Factor: " + TOKEN_KIND_NAMES[token->kind]); 
            return nullptr; 
        }
};