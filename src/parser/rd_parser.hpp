#pragma once

#include "int_conv.hpp"
#include "parser.hpp"
#include "print.hpp"

static std::unordered_map<TokenKind, UnaryType> TOKEN_TO_UNARY_TYPE
{
    { TokenKind::Negation, UnaryType::Negation },
    { TokenKind::BWComplement, UnaryType::BWComplement },
    { TokenKind::LogicalNegation, UnaryType::LogicalNegation }
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
        Program* ParseProgram()
        {
            auto func = ParseFunction(); 
            if (func == nullptr) return nullptr; 
            Program* p = new Program; 
            p->function = func; 
            return p; 
        }

        Function* ParseFunction()
        {
            auto token = NextToken();
            if (token->kind != TokenKind::Keyword || token->value != "int") return nullptr; 
            token = NextToken(); 
            if (token->kind != TokenKind::Identifier) return nullptr; 
            std::string name = token->value; 
            token = NextToken(); 
            if (token->kind != TokenKind::LeftParenthesis) return nullptr;
            token = NextToken(); 
            if (token->kind != TokenKind::RightParenthesis) return nullptr; 
            token = NextToken(); 
            if (token->kind != TokenKind::LeftBrace) return nullptr; 
            auto statement = ParseStatement(); 
            if (statement == nullptr) return nullptr; 
            token = NextToken(); 
            if (token->kind != TokenKind::RightBrace) return nullptr; 
            Function* func = new Function;
            func->name = name; 
            func->statement = statement; 
            return func; 
        }

        Statement* ParseStatement()
        {
            auto token = NextToken(); 
            if (token->kind != TokenKind::Keyword || token->value != "return") return nullptr; 
            auto expr = ParseExpression(); 
            if (expr == nullptr) return nullptr;
            token = NextToken(); 
            if (token->kind != TokenKind::Semicolon) return nullptr; 
            auto ret = new Return; 
            ret->expr = expr; 
            return ret; 
        }

        Expression* ParseExpression()
        {
            auto token = NextToken();
            if (token->kind == TokenKind::IntConstant || token->kind == TokenKind::HexConstant)
            {
                auto expr = new IntExpr; 
                expr->value = parse_c_int(token->value); 
                return expr; 
            } else if (TOKEN_TO_UNARY_TYPE.find(token->kind) != TOKEN_TO_UNARY_TYPE.end()) 
            {
                auto op = new UnaryOp;
                op->type = TOKEN_TO_UNARY_TYPE[token->kind]; 
                op->expr = ParseExpression(); 
                return op; 
            } else return nullptr; 
        }
};