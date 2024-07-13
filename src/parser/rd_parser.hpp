#pragma once

#include "parser.hpp"
#include "int_conv.hpp"

// Recursive-Descent Parser

class RDParser : public Parser
{
    public:
        RDParser(std::vector<Token>& tokens) : Parser(tokens) 
        {
        }

        AbstractSyntax* parse() override
        {
            return parseProgram(); 
        }

    private:
        Program* parseProgram()
        {
            auto func = parseFunction(); 
            if (func == nullptr) return nullptr; 
            Program* p = new Program; 
            p->function = func; 
            return p; 
        }

        Function* parseFunction()
        {
            auto token = nextToken();
            if (token->kind != TokenKind::Keyword || token->value != "int") return nullptr; 
            token = nextToken(); 
            if (token->kind != TokenKind::Identifier) return nullptr; 
            std::string name = token->value; 
            token = nextToken(); 
            if (token->kind != TokenKind::LeftParenthesis) return nullptr;
            token = nextToken(); 
            if (token->kind != TokenKind::RightParenthesis) return nullptr; 
            token = nextToken(); 
            if (token->kind != TokenKind::LeftBrace) return nullptr; 
            auto statement = parseStatement(); 
            if (statement == nullptr) return nullptr; 
            token = nextToken(); 
            if (token->kind != TokenKind::RightBrace) return nullptr; 
            Function* func = new Function;
            func->name = name; 
            func->statement = statement; 
            return func; 
        }

        Statement* parseStatement()
        {
            auto token = nextToken(); 
            if (token->kind != TokenKind::Keyword || token->value != "return") return nullptr; 
            auto expr = parseExpresssion(); 
            if (expr == nullptr) return nullptr;
            token = nextToken(); 
            if (token->kind != TokenKind::Semicolon) return nullptr; 
            auto ret = new Return; 
            ret->expr = expr; 
            return ret; 
        }

        Expression* parseExpresssion()
        {
            auto token = nextToken();
            if (token->kind == TokenKind::IntConstant || token->kind == TokenKind::HexConstant)
            {
                auto expr = new IntExpr; 
                expr->value = parse_c_int(token->value); 
                return expr; 
            } else return nullptr; 
        }
};