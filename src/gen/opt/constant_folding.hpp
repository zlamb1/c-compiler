#pragma once

#include <optional>

namespace
{
    static std::unordered_map<std::string, int> variables;

    static bool is_constant(AbstractSyntax* syntax)
    {
        if (syntax == nullptr) return false;
        return syntax->type() == SyntaxType::IntConstant; 
    }

    static std::optional<int> get_value(Expression* expr)
    {
        if (expr != nullptr)
        {
            switch (expr->type())
            {
                case SyntaxType::IntConstant:
                    return dynamic_cast<IntConstant*>(expr)->value;
                case SyntaxType::VariableRef:
                {
                    auto var = dynamic_cast<VariableRef*>(expr);
                    if (variables.find(var->name) != variables.end())
                    {
                        return variables[var->name]; 
                    }
                    break;
                }
                case SyntaxType::BinaryOp:
                    auto op = dynamic_cast<BinaryOp*>(expr); 
                    if (op->opType == BinaryOpType::Comma)
                        return get_value(op->rvalue); 
                    break;
            }
        }
        return std::nullopt; 
    }

    static Expression* FoldConstants(Expression* expr)
    {
        if (expr == nullptr) return expr; 
        switch (expr->type())
        {
            case SyntaxType::UnaryOp:
            {
                auto op = dynamic_cast<UnaryOp*>(expr);
                op->expr = FoldConstants(op->expr); 
                auto constant = get_value(op->expr);
                if (constant)
                {
                    switch (op->opType)
                    {
                        case UnaryOpType::Negation: 
                            return new IntConstant(-constant.value());
                        case UnaryOpType::Complement:
                            return new IntConstant(~constant.value()); 
                        case UnaryOpType::LogicalNegation:
                            return new IntConstant(!constant.value());  
                    }
                }
                break; 
            }
            case SyntaxType::BinaryOp:
            {
                auto op = dynamic_cast<BinaryOp*>(expr);
                op->lvalue = FoldConstants(op->lvalue); 
                op->rvalue = FoldConstants(op->rvalue); 
                auto lconstant = get_value(op->lvalue); 
                auto rconstant = get_value(op->rvalue);
                if (lconstant && rconstant)
                {
                    auto lvalue = lconstant.value();
                    auto rvalue = rconstant.value(); 
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
                } else if (lconstant)
                    return new BinaryOp(op->opType, new IntConstant(lconstant.value()), op->rvalue); 
                else if (rconstant)
                    return new BinaryOp(op->opType, op->lvalue, new IntConstant(rconstant.value())); 
                break; 
            }
        }
        return expr; 
    }

    // Evaluate unary/binary operations that can be reduced to constants in the abstract syntax tree.
    static AbstractSyntax* EvaluateSyntax(AbstractSyntax* syntax)
    {
        switch (syntax->type())
        {
            case SyntaxType::Program:
            {
                auto program = dynamic_cast<Program*>(syntax);
                EvaluateSyntax(program->function); 
                break; 
            }
            case SyntaxType::Function:
            {
                auto function = dynamic_cast<Function*>(syntax); 
                for (auto statement : function->statements)
                    EvaluateSyntax(statement); 
                break; 
            }
            case SyntaxType::StatementExpression:
            {
                auto statementExpr = dynamic_cast<StatementExpression*>(syntax);
                statementExpr->expr = FoldConstants(statementExpr->expr);
                break;
            }
            case SyntaxType::Declaration:
            {
                auto decl = dynamic_cast<Declaration*>(syntax);
                for (auto var : decl->variables)
                {
                    if (var.expr == nullptr) variables[var.name] = 0; 
                    else 
                    {
                        var.expr = FoldConstants(var.expr); 
                        auto constant = get_value(var.expr);
                        if (constant) variables[var.name] = constant.value(); 
                    }
                }
                break;
            }
            case SyntaxType::Return:
            {
                auto ret = dynamic_cast<Return*>(syntax); 
                ret->expr = FoldConstants(ret->expr);
                break; 
            }
        }

        return syntax; 
    }
};

static void OptimizeTree(AbstractSyntax* syntax)
{
    variables.clear(); 
    EvaluateSyntax(syntax); 
}