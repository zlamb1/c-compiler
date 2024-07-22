#pragma once

#include "int_conv.hpp"
#include "maps.hpp"
#include "parser.hpp"
#include "print.hpp"

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
                PrettyPrintAST(ast);
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
            auto function = new Function(name); 
            token = PeekToken();
            bool foundReturn = false;
            while (token->kind != TokenKind::RightBrace)
            {
                if (token->kind == TokenKind::None) ExceptParse("Unclosed Block: expected '}'");
                auto statement = ParseStatement();
                // discard statements encountered after return
                if (!foundReturn) function->statements.emplace_back(statement); 
                if (statement->type() == SyntaxType::Return) foundReturn = true; 
                token = PeekToken(); 
            }
            return function;
        }

        Statement* ParseStatement()
        {
            auto token = PeekToken(); 
            Statement* statement; 
            if (token->kind == TokenKind::Keyword)
            {
                ConsumeToken(); 
                if (token->value == "return") statement = new Return(ParseExpression()); 
                else if (token->value == "int") statement = ParseDeclaration(); 
            } else if (token->kind == TokenKind::Semicolon) statement = new StatementExpression(nullptr);  
            else statement = new StatementExpression(ParseExpression()); 
            token = NextToken(); 
            if (token->kind != TokenKind::Semicolon) ExceptParse("Unclosed Statement: expected ';'");
            return statement; 
        }

        Declaration* ParseDeclaration()
        {
            auto decl = new Declaration(); 
            decl->variables.emplace_back(ParseVariable()); 
            auto token = PeekToken(); 
            while (token->kind == TokenKind::Comma)
            {
                ConsumeToken();
                decl->variables.emplace_back(ParseVariable()); 
                token = PeekToken(); 
            }
            return decl;
        }

        Variable ParseVariable()
        {
            auto token = NextToken(); 
            if (token->kind != TokenKind::Identifier) ExceptParse("Expected Symbol Identifier"); 
            auto name = token->value; 
            token = PeekToken(); 
            if (token->kind == TokenKind::Equal)
            {
                ConsumeToken();
                // the comma operator is not used for declarations unless wrapped in parenthesis
                return Variable(name, ParseAssignmentExpression()); 
            } else return Variable(name, nullptr); 
        }

        Expression* ParseExpression()
        {
            auto expr = ParseAssignmentExpression(); 
            auto token = PeekToken(); 
            if (token->kind == TokenKind::None) ExceptParse("Incomplete Expression"); 
            while (token->kind == TokenKind::Comma)
            {
                ConsumeToken(); 
                auto nextExpr = ParseAssignmentExpression(); 
                expr = new BinaryOp(BinaryOpType::Comma, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression* ParseAssignmentExpression()
        {
            auto token = PeekToken(); 
            if (token->kind == TokenKind::Identifier)
            {
                ConsumeToken(); 
                auto lvalue = token->value; 
                token = PeekToken(); 
                if (token->kind != TokenKind::Equal)
                {
                    if (TOKEN_TO_ASSIGNMENT_OP_TYPE.find(token->kind) != TOKEN_TO_ASSIGNMENT_OP_TYPE.end())
                    {
                        ConsumeToken(); 
                        return new AssignmentOp(TOKEN_TO_ASSIGNMENT_OP_TYPE[token->kind], lvalue, ParseExpression()); 
                    }
                    RevertToken(); 
                    return ParseLogicalOrExpression(); 
                }
                ConsumeToken(); 
                return new Assignment(lvalue, ParseExpression());
            } else return ParseLogicalOrExpression(); 
        }

        Expression* ParseLogicalOrExpression()
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
                return new IntConstant(parse_c_int(token->value)); 
            else if (token->kind == TokenKind::Identifier) 
                return new VariableRef(token->value);
            else ExceptParse("Invalid Factor: " + TOKEN_KIND_NAMES[token->kind]); 
            return nullptr; 
        }
};