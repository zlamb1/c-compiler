#include "rd_parser.hpp"

AbstractSyntax::Ref RDParser::ParseFile(const std::string& filepath)
{
    m_Tokens = m_Lexer->LexFile(filepath); 
    //std::cout << "<--- Lexical Analysis --." << std::endl;
    //for (auto& token : m_Tokens)
    //    std::cout << TOKEN_KIND_NAMES[token.kind] << ": " << (!token.value.empty() ? token.value : "NULL") << std::endl;
    return ParseProgram(); 
}

void RDParser::LogTree(std::ostream& out, const AbstractSyntax::Ref root) const
{
    print_ast(out, root);
}

Program::Ref RDParser::ParseProgram()
{
    return CreateRef<Program>(ParseFunction()); 
}

Function::Ref RDParser::ParseFunction()
{
    auto token = NextToken();
    if (token.kind != TokenKind::Keyword || token.value != "int") ExceptParse("error: Invalid return type", token); 
    token = NextToken(); 
    if (token.kind != TokenKind::Identifier) ExceptParse("error: Excepted function identifier", token); 
    std::string name = token.value; 
    lparen();
    rparen();
    return CreateRef<Function>(name, ParseCompoundBlock());
}

// Statement | Declaration
Statement::Ref RDParser::ParseBlockItem()
{
    auto token = PeekToken(); 
    if (token.kind == TokenKind::Keyword && token.value == "int")
    {
        ConsumeToken(); 
        return ParseDeclaration(); 
    } else return ParseStatement(); 
}

Statement::Ref RDParser::ParseStatement()
{
    auto token = PeekToken(); 
    Statement::Ref statement; 
    if (token.kind == TokenKind::Keyword)
    {
        ConsumeToken(); 
        if (token.value == "return") statement = CreateRef<Return>(ParseExpression()); 
        else if (token.value == "if") return ParseIfStatement(); 
        else if (token.value == "while") return ParseWhileStatement(); 
        else if (token.value == "for") return ParseForStatement();
        else if (token.value == "do") statement = ParseDoWhileStatement(); 
        else if (token.value == "break") statement = CreateRef<BreakStatement>();
        else if (token.value == "continue") statement = CreateRef<ContinueStatement>();  
    } else if (token.kind == TokenKind::LeftBrace) return ParseCompoundBlock(); 
    else statement = CreateRef<StatementExpression>(ParseNullExpression());
    semicolon();
    return statement; 
}

Variable RDParser::ParseVariable()
{
    auto token = NextToken(); 
    if (token.kind != TokenKind::Identifier) ExceptParse("error: Expected identifier", token); 
    auto name = token.value; 
    token = PeekToken(); 
    if (token.kind == TokenKind::Equal)
    {
        ConsumeToken();
        // the comma operator is applicable to declarations unless wrapped in parenthesis
        return Variable(name, ParseAssignmentExpression()); 
    } else return Variable(name, nullptr); 
}

Declaration::Ref RDParser::ParseDeclaration()
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
    semicolon();
    return decl;
}

CompoundBlock::Ref RDParser::ParseCompoundBlock()
{
    lbrace();
    auto block = CreateRef<CompoundBlock>(); 
    auto token = PeekToken(); 
    while (token.kind != TokenKind::RightBrace)
    {
        if (token.kind == TokenKind::None) ExceptParse("error: expected '}'", token);
        block->statements.emplace_back(ParseBlockItem());
        token = PeekToken(); 
    }
    ConsumeToken(); 
    return block; 
}

Conditional RDParser::ParseIfCondition()
{
    lparen();
    auto condition = ParseExpression(); 
    rparen();
    return Conditional(condition, ParseStatement()); 
}

IfStatement::Ref RDParser::ParseIfStatement()
{
    auto statement = CreateRef<IfStatement>(ParseIfCondition()); 
    auto token = PeekToken(); 
    while (token.kind == TokenKind::Keyword && token.value == "else")
    {
        ConsumeToken();
        token = PeekToken();
        if (token.kind == TokenKind::Keyword && token.value == "if")
        {
            ConsumeToken(); 
            statement->else_ifs.emplace_back(ParseIfCondition()); 
        } else {
            statement->else_statement = ParseStatement();  
            break;
        }
        token = PeekToken(); 
    }
    return statement; 
}

DoWhileStatement::Ref RDParser::ParseDoWhileStatement()
{
    auto body = ParseStatement();
    auto token = NextToken();
    if (token.kind != TokenKind::Keyword || token.value != "while") 
        ExceptParse("error: Expected keyword 'while'", token);
    lparen();
    auto condition = ParseExpression();
    rparen();
    return CreateRef<DoWhileStatement>(body, condition); 
}

Statement::Ref RDParser::ParseForStatement()
{
    lparen();
    auto token = PeekToken();
    if (token.kind == TokenKind::Keyword && token.value == "int")
    {
        ConsumeToken();
        auto declaration = ParseDeclaration(); 
        auto condition = ParseNullExpression(); 
        if (condition->type() == SyntaxType::Null) condition = CreateRef<IntConstant>(1);
        semicolon();
        token = PeekToken();
        Expression::Ref post_expression;
        if (token.kind == TokenKind::RightParenthesis)
        {
            ConsumeToken(); 
            post_expression = CreateRef<NullExpression>();
        } else {
            post_expression = ParseExpression(); 
            rparen();
        }
        return CreateRef<ForDeclStatement>(declaration, condition, post_expression, ParseStatement()); 
    } else {
        auto expression = ParseNullExpression(); 
        semicolon();
        auto condition = ParseNullExpression(); 
        if (condition->type() == SyntaxType::Null) condition = CreateRef<IntConstant>(1);
        semicolon();
        token = PeekToken();
        Expression::Ref post_expression;
        if (token.kind == TokenKind::RightParenthesis)
        {
            ConsumeToken(); 
            post_expression = CreateRef<NullExpression>();
        } else {
            post_expression = ParseExpression(); 
            rparen();
        }
        return CreateRef<ForStatement>(expression, condition, post_expression, ParseStatement()); 
    }
}

WhileStatement::Ref RDParser::ParseWhileStatement()
{
    lparen();
    auto condition = ParseExpression(); 
    rparen();
    auto body = ParseStatement(); 
    return CreateRef<WhileStatement>(condition, body);
}

Expression::Ref RDParser::ParseNullExpression()
{
    auto token = PeekToken(); 
    if (token.kind == TokenKind::Semicolon)
        return CreateRef<NullExpression>(); 
    else return ParseExpression(); 
}

Expression::Ref RDParser::ParseExpression()
{
    auto expr = ParseAssignmentExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
    while (token.kind == TokenKind::Comma)
    {
        ConsumeToken(); 
        auto nextExpr = ParseAssignmentExpression(); 
        expr = CreateRef<BinaryOp>(BinaryOpType::Comma, expr, nextExpr); 
        token = PeekToken();
    }
    return expr; 
}

Expression::Ref RDParser::ParseAssignmentExpression()
{
    auto token = PeekToken(); 
    if (token.kind == TokenKind::Identifier)
    {
        ConsumeToken(); 
        auto lvalue = token.value; 
        token = PeekToken(); 
        switch (token.kind)
        {
            case TokenKind::Equal:
                ConsumeToken(); 
                return CreateRef<Assignment>(lvalue, ParseExpression());
            case TokenKind::Semicolon:
                RevertToken(); 
                return ParseTernaryExpression();
            default:
                if (TOKEN_TO_ASSIGNMENT_OP_TYPE.find(token.kind) != TOKEN_TO_ASSIGNMENT_OP_TYPE.end())
                {
                    ConsumeToken(); 
                    return CreateRef<AssignmentOp>(TOKEN_TO_ASSIGNMENT_OP_TYPE[token.kind], lvalue, ParseExpression()); 
                }
                RevertToken();
                return ParseTernaryExpression(); 
        }
    } else return ParseTernaryExpression(); 
}

Expression::Ref RDParser::ParseTernaryExpression()
{
    auto expr = ParseLogicalOrExpression();
    auto token = PeekToken();
    if (token.kind == TokenKind::QuestionMark)
    {
        ConsumeToken();
        auto lvalue = ParseExpression();
        colon();
        auto rvalue = ParseTernaryExpression();
        return CreateRef<TernaryOp>(expr, lvalue, rvalue);
    }
    return expr; 
}

Expression::Ref RDParser::ParseLogicalOrExpression()
{
    auto expr = ParseLogicalAndExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseLogicalAndExpression()
{
    auto expr = ParseBitwiseOrExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseBitwiseOrExpression()
{
    auto expr = ParseBitwiseXORExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseBitwiseXORExpression()
{
    auto expr = ParseBitwiseAndExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseBitwiseAndExpression()
{
    auto expr = ParseEqualityExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseEqualityExpression()
{
    auto expr = ParseRelationalExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseRelationalExpression()
{
    auto expr = ParseShiftExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseShiftExpression()
{
    auto expr = ParseAdditiveExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseAdditiveExpression()
{
    auto term = ParseTerm(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete expression", token); 
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

Expression::Ref RDParser::ParseTerm()
{
    auto expr = ParseUnaryExpression(); 
    auto token = PeekToken(); 
    if (token.kind == TokenKind::None) ExceptParse("error: Incomplete term", token); 
    while (token.kind == TokenKind::Asterisk || token.kind == TokenKind::Slash || token.kind == TokenKind::Percent)
    {
        ConsumeToken(); 
        auto type = TOKEN_TO_BINARY_TYPE[token.kind]; 
        auto nextExpr = ParseUnaryExpression(); 
        expr = CreateRef<BinaryOp>(type, expr, nextExpr); 
        token = PeekToken(); 
    }
    return expr; 
}

Expression::Ref RDParser::ParseUnaryExpression()
{
    auto token = PeekToken();
    if (TOKEN_TO_UNARY_TYPE.find(token.kind) != TOKEN_TO_UNARY_TYPE.end())
    {
        auto type = TOKEN_TO_UNARY_TYPE[token.kind];
        ConsumeToken();
        switch (token.kind)
        {
            case TokenKind::PlusPlus:
            case TokenKind::MinusMinus:
            {
                token = NextToken(); 
                if (token.kind != TokenKind::Identifier) 
                    ExceptParse("error: lvalue required for operator '" + TOKEN_KIND_NAMES[token.kind] + "'", token);
                return CreateRef<UnaryOp>(type, CreateRef<VariableRef>(token.value)); 
            }
        }
        return CreateRef<UnaryOp>(type, ParseUnaryExpression()); 
    } else return ParseFactor(); 
}

Expression::Ref RDParser::ParseFactor()
{
    auto token = NextToken();
    if (token.kind == TokenKind::LeftParenthesis)
    {
        auto expr = ParseExpression(); 
        rparen();
        return expr; 
    } else if (token.kind == TokenKind::IntConstant || token.kind == TokenKind::HexConstant)
        return CreateRef<IntConstant>(parse_c_int(token.value)); 
    else if (token.kind == TokenKind::Identifier) 
    {
        auto value = token.value;
        token = PeekToken();
        // parse postfix ops
        switch (token.kind)
        {
            case TokenKind::PlusPlus:
            case TokenKind::MinusMinus:
                ConsumeToken(); 
                auto op = token.kind == TokenKind::PlusPlus ? UnaryOpType::PostfixIncrement :
                    UnaryOpType::PostfixDecrement;
                return CreateRef<UnaryOp>(op, CreateRef<VariableRef>(value)); 
        }
        return CreateRef<VariableRef>(value);
    }
    else ExceptParse("error: Unexpected token '" + TOKEN_KIND_NAMES[token.kind] + "'", token); 
    return nullptr; 
}