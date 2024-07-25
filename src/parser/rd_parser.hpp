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

        AbstractSyntax::Ref ParseFile(const std::string& filepath) override
        {
            m_Tokens = m_Lexer->LexFile(filepath); 
            //std::cout << "<--- Lexical Analysis --." << std::endl;
            //for (auto& token : m_Tokens)
            //    std::cout << TOKEN_KIND_NAMES[token.kind] << ": " << (!token.value.empty() ? token.value : "NULL") << std::endl;
            return ParseProgram(); 
        }

        void PrintTree(AbstractSyntax::Ref ast) override
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

        Program::Ref ParseProgram()
        {
            return CreateRef<Program>(ParseFunction()); 
        }

        Function::Ref ParseFunction()
        {
            auto token = NextToken();
            if (token.kind != TokenKind::Keyword || token.value != "int") ExceptParse("error: Invalid return type"); 
            token = NextToken(); 
            if (token.kind != TokenKind::Identifier) ExceptParse("error: Excepted function identifier"); 
            std::string name = token.value; 
            token = NextToken(); 
            if (token.kind != TokenKind::LeftParenthesis) ExceptParse("error: Expected '('"); 
            token = NextToken(); 
            if (token.kind != TokenKind::RightParenthesis) ExceptParse("error: Expected ')'"); 
            return CreateRef<Function>(name, ParseCompoundBlock());
        }

        // Statement | Declaration
        Statement::Ref ParseBlockItem()
        {
            auto token = PeekToken(); 
            if (token.kind == TokenKind::Keyword && token.value == "int")
            {
                ConsumeToken(); 
                return ParseDeclaration(); 
            } else return ParseStatement(); 
        }

        Statement::Ref ParseStatement()
        {
            auto token = PeekToken(); 
            Statement::Ref statement; 
            if (token.kind == TokenKind::Keyword)
            {
                ConsumeToken(); 
                if (token.value == "return") statement = CreateRef<Return>(ParseExpression()); 
                else if (token.value == "if") return ParseIfStatement(); 
            } else if (token.kind == TokenKind::LeftBrace) return ParseCompoundBlock(); 
            else if (token.kind == TokenKind::Semicolon) statement = CreateRef<StatementExpression>(nullptr);  
            else statement = CreateRef<StatementExpression>(ParseExpression()); 
            token = NextToken(); 
            if (token.kind != TokenKind::Semicolon) ExceptParse("error: expected ';'");
            return statement; 
        }

        Declaration::Ref ParseDeclaration()
        {
            // TODO: introduce type system
            // for now only int (4-bytes) exists
            auto decl = CreateRef<Declaration>(4); 
            decl->variables.emplace_back(ParseVariable()); 
            auto token = PeekToken(); 
            while (token.kind == TokenKind::Comma)
            {
                ConsumeToken();
                decl->variables.emplace_back(ParseVariable()); 
                token = PeekToken(); 
            }
            token = NextToken(); 
            if (token.kind != TokenKind::Semicolon) ExceptParse("error: Expected ';'");
            return decl;
        }

        Variable ParseVariable()
        {
            auto token = NextToken(); 
            if (token.kind != TokenKind::Identifier) ExceptParse("error: Expected identifier"); 
            auto name = token.value; 
            token = PeekToken(); 
            if (token.kind == TokenKind::Equal)
            {
                ConsumeToken();
                // the comma operator is not used for declarations unless wrapped in parenthesis
                return Variable(name, ParseAssignmentExpression()); 
            } else return Variable(name, nullptr); 
        }

        CompoundBlock::Ref ParseCompoundBlock()
        {
            auto token = NextToken();
            if (token.kind != TokenKind::LeftBrace) ExceptParse("error: expected '{'");
            auto block = CreateRef<CompoundBlock>(); 
            token = PeekToken(); 
            while (token.kind != TokenKind::RightBrace)
            {
                if (token.kind == TokenKind::None) ExceptParse("error: expected '}'");
                block->statements.emplace_back(ParseBlockItem());
                token = PeekToken(); 
            }
            ConsumeToken(); 
            return block; 
        }

        Conditional ParseConditional()
        {
            auto token = NextToken();
            if (token.kind != TokenKind::LeftParenthesis) ExceptParse("error: Expected '('");
            auto condition = ParseExpression(); 
            token = NextToken();
            if (token.kind != TokenKind::RightParenthesis) ExceptParse("error: Expected ')'"); 
            return Conditional(condition, ParseStatement()); 
        }

        IfStatement::Ref ParseIfStatement()
        {
            auto statement = CreateRef<IfStatement>(ParseConditional()); 
            auto token = PeekToken(); 
            while (token.kind == TokenKind::Keyword && token.value == "else")
            {
                ConsumeToken();
                token = PeekToken();
                if (token.kind == TokenKind::Keyword && token.value == "if")
                {
                    ConsumeToken(); 
                    statement->else_ifs.emplace_back(ParseConditional()); 
                } else
                {
                    statement->else_statement = ParseStatement();  
                    break;
                }
                token = PeekToken(); 
            }
            return statement; 
        }

        Expression::Ref ParseExpression()
        {
            auto expr = ParseAssignmentExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::Comma)
            {
                ConsumeToken(); 
                auto nextExpr = ParseAssignmentExpression(); 
                expr = CreateRef<BinaryOp>(BinaryOpType::Comma, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseAssignmentExpression()
        {
            auto token = PeekToken(); 
            if (token.kind == TokenKind::Identifier)
            {
                ConsumeToken(); 
                auto lvalue = token.value; 
                token = PeekToken(); 
                if (token.kind != TokenKind::Equal)
                {
                    if (TOKEN_TO_ASSIGNMENT_OP_TYPE.find(token.kind) != TOKEN_TO_ASSIGNMENT_OP_TYPE.end())
                    {
                        ConsumeToken(); 
                        return CreateRef<AssignmentOp>(TOKEN_TO_ASSIGNMENT_OP_TYPE[token.kind], lvalue, ParseExpression()); 
                    }
                    RevertToken(); 
                    return ParseLogicalOrExpression(); 
                }
                ConsumeToken(); 
                return CreateRef<Assignment>(lvalue, ParseExpression());
            } else return ParseLogicalOrExpression(); 
        }

        Expression::Ref ParseLogicalOrExpression()
        {
            auto expr = ParseLogicalAndExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::DoublePipe)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseLogicalAndExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseLogicalAndExpression()
        {
            auto expr = ParseBitwiseOrExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::DoubleAmpersand)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseBitwiseOrExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseBitwiseOrExpression()
        {
            auto expr = ParseBitwiseXORExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::Pipe)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseBitwiseXORExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseBitwiseXORExpression()
        {
            auto expr = ParseBitwiseAndExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::Caret)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseBitwiseAndExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseBitwiseAndExpression()
        {
            auto expr = ParseEqualityExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::Ampersand)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseEqualityExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseEqualityExpression()
        {
            auto expr = ParseRelationalExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::DoubleEquals || token.kind == TokenKind::NotEqual)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseRelationalExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseRelationalExpression()
        {
            auto expr = ParseShiftExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::LessThan || token.kind == TokenKind::LessThanOrEqual ||
                   token.kind == TokenKind::GreaterThan || token.kind == TokenKind::GreaterThanOrEqual)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseShiftExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseShiftExpression()
        {
            auto expr = ParseAdditiveExpression(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::LeftShift || token.kind == TokenKind::RightShift)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind];
                auto nextExpr = ParseAdditiveExpression(); 
                expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
                token = PeekToken();
            }
            return expr; 
        }

        Expression::Ref ParseAdditiveExpression()
        {
            auto term = ParseTerm(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression"); 
            while (token.kind == TokenKind::Plus || token.kind == TokenKind::Minus)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind]; 
                auto nextTerm = ParseTerm(); 
                term = CreateRef<BinaryOp>(type, term, nextTerm); 
                token = PeekToken(); 
            }
            return term; 
        }

        Expression::Ref ParseTerm()
        {
            auto factor = ParseFactor(); 
            auto token = PeekToken(); 
            if (token.kind == TokenKind::None) ExceptParse("error: Incomplete term"); 
            while (token.kind == TokenKind::Asterisk || token.kind == TokenKind::Slash || token.kind == TokenKind::Percent)
            {
                ConsumeToken(); 
                auto type = TOKEN_TO_BINARY_TYPE[token.kind]; 
                auto nextFactor = ParseFactor(); 
                factor = CreateRef<BinaryOp>(type, factor, nextFactor); 
                token = PeekToken(); 
            }
            return factor; 
        }

        Expression::Ref ParseFactor()
        {
            auto token = NextToken();
            if (token.kind == TokenKind::LeftParenthesis)
            {
                auto expr = ParseExpression(); 
                token = NextToken();
                if (token.kind != TokenKind::RightParenthesis) ExceptParse("error: expected ')'"); 
                return expr; 
            } else if (TOKEN_TO_UNARY_TYPE.find(token.kind) != TOKEN_TO_UNARY_TYPE.end())
            {
                auto factor = ParseFactor(); 
                return CreateRef<UnaryOp>(TOKEN_TO_UNARY_TYPE[token.kind], factor); 
            } else if (token.kind == TokenKind::IntConstant || token.kind == TokenKind::HexConstant)
                return CreateRef<IntConstant>(parse_c_int(token.value)); 
            else if (token.kind == TokenKind::Identifier) 
                return CreateRef<VariableRef>(token.value);
            else ExceptParse("error: Unexpected token '" + TOKEN_KIND_NAMES[token.kind] + "'"); 
            return nullptr; 
        }
};