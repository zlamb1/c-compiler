#pragma once

namespace
{
    static bool is_constant(AbstractSyntax* syntax)
    {
        return syntax->type() == SyntaxType::IntConstant; 
    }

    static Expression* FoldExpression(Expression* expr)
    {
        switch (expr->type())
        {
            case SyntaxType::UnaryOp:
            {
                auto op = dynamic_cast<UnaryOp*>(expr);
                op->expr = FoldExpression(op->expr); 
                if (is_constant(op->expr))
                {
                    auto constant = dynamic_cast<IntConstant*>(op->expr); 
                    switch (op->opType)
                    {
                        case UnaryOpType::Negation:
                            return new IntConstant(-constant->value);
                        case UnaryOpType::Complement:
                            return new IntConstant(~constant->value);
                        case UnaryOpType::LogicalNegation:
                            return new IntConstant(!constant->value); 
                    }
                }
                break; 
            }
            case SyntaxType::BinaryOp:
            {
                auto op = dynamic_cast<BinaryOp*>(expr);
                op->lvalue = FoldExpression(op->lvalue); 
                op->rvalue = FoldExpression(op->rvalue); 
                if (is_constant(op->lvalue) && is_constant(op->rvalue))
                {
                    auto lvalue = dynamic_cast<IntConstant*>(op->lvalue)->value;
                    auto rvalue = dynamic_cast<IntConstant*>(op->rvalue)->value; 
                    switch (op->opType)
                    {
                        case BinaryOpType::Addition:
                            return new IntConstant(lvalue + rvalue);
                        case BinaryOpType::Subtraction:
                            return new IntConstant(lvalue - rvalue);
                        case BinaryOpType::Multiplication:
                            return new IntConstant(lvalue * rvalue);
                        case BinaryOpType::Division:
                            return new IntConstant(lvalue / rvalue);
                        case BinaryOpType::LogicalOr:
                            return new IntConstant(lvalue || rvalue);
                        case BinaryOpType::LogicalAnd:
                            return new IntConstant(lvalue && rvalue);
                        case BinaryOpType::Equal:
                            return new IntConstant(lvalue == rvalue);
                        case BinaryOpType::NotEqual:
                            return new IntConstant(lvalue != rvalue);
                        case BinaryOpType::Remainder:
                            return new IntConstant(lvalue % rvalue); 
                        case BinaryOpType::LessThan:
                            return new IntConstant(lvalue < rvalue);
                        case BinaryOpType::LessThanOrEqual:
                            return new IntConstant(lvalue <= rvalue);
                        case BinaryOpType::GreaterThan:
                            return new IntConstant(lvalue > rvalue);
                        case BinaryOpType::GreaterThanOrEqual:
                            return new IntConstant(lvalue >= rvalue); 
                        case BinaryOpType::BitwiseOr:
                            return new IntConstant(lvalue | rvalue);
                        case BinaryOpType::BitwiseAnd:
                            return new IntConstant(lvalue & rvalue); 
                        case BinaryOpType::BitwiseXOR:
                            return new IntConstant(lvalue ^ rvalue); 
                        case BinaryOpType::BitwiseLeftShift:
                            return new IntConstant(lvalue << rvalue);
                        case BinaryOpType::BitwiseRightShift:
                            return new IntConstant(lvalue >> rvalue); 
                    }
                }
                break; 
            }
        }
        return expr; 
    }
};

// Evaluate unary/binary operations that can be reduced to constants in the abstract syntax tree.

static AbstractSyntax* FoldConstants(AbstractSyntax* syntax)
{
    switch (syntax->type())
    {
        case SyntaxType::Program:
        {
            auto program = dynamic_cast<Program*>(syntax);
            FoldConstants(program->function); 
            break; 
        }
        case SyntaxType::Function:
        {
            auto function = dynamic_cast<Function*>(syntax); 
            for (auto statement : function->statements)
                FoldConstants(statement); 
            break; 
        }
        case SyntaxType::StatementExpression:
        {
            auto statementExpr = dynamic_cast<StatementExpression*>(syntax);
            if (statementExpr->expr) statementExpr->expr = FoldExpression(statementExpr->expr);
            break;
        }
        case SyntaxType::Return:
        {
            auto ret = dynamic_cast<Return*>(syntax); 
            auto expr = FoldExpression(ret->expr);
            ret->expr = expr; 
            break; 
        }
    }

    return syntax; 
}