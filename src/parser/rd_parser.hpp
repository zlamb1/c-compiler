#pragma once

#include <string>

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

    AbstractSyntax::Ref ParseFile(const std::string& filepath) override;
    void LogTree(std::ostream& out, const AbstractSyntax::Ref root) const override;

private:
    Program::Ref ParseProgram();
    Function::Ref ParseFunction();
    Statement::Ref ParseBlockItem();
    Statement::Ref ParseStatement();
    Variable ParseVariable();
    Declaration::Ref ParseDeclaration();
    CompoundBlock::Ref ParseCompoundBlock();
    Conditional ParseIfCondition();
    IfStatement::Ref ParseIfStatement();
    DoWhileStatement::Ref ParseDoWhileStatement();
    Statement::Ref ParseForStatement();
    WhileStatement::Ref ParseWhileStatement();
    Expression::Ref ParseNullExpression();
    Expression::Ref ParseExpression();
    Expression::Ref ParseAssignmentExpression();
    Expression::Ref ParseTernaryExpression();
    Expression::Ref ParseLogicalOrExpression();
    Expression::Ref ParseLogicalAndExpression();
    Expression::Ref ParseBitwiseOrExpression();
    Expression::Ref ParseBitwiseXORExpression();
    Expression::Ref ParseBitwiseAndExpression();
    Expression::Ref ParseEqualityExpression();
    Expression::Ref ParseRelationalExpression();
    Expression::Ref ParseShiftExpression();
    Expression::Ref ParseAdditiveExpression();
    Expression::Ref ParseTerm();
    Expression::Ref ParseUnaryExpression();
    Expression::Ref ParseFactor();
};