#pragma once

#include <optional>

namespace
{
    // take care that variables are popped from map when their value is no longer known
    // i.e. a = foo();
    static std::unordered_map<std::string, int> variables;

    static bool is_constant(AbstractSyntax::Ref syntax)
    {
        if (syntax == nullptr) return false;
        return syntax->type() == SyntaxType::IntConstant; 
    }

    static bool variable_exists(const std::string& var)
    {
        return variables.find(var) != variables.end(); 
    }

    static void remove_variable(const std::string& var)
    {
        auto iter = variables.find(var); 
        if (iter != variables.end())
            variables.erase(iter);
    }

    static std::optional<int> get_value(Expression::Ref expr)
    {
        switch (expr->type())
        {
            case SyntaxType::IntConstant:
                return AbstractSyntax::RefCast<IntConstant>(expr)->value;
            case SyntaxType::VariableRef:
            {
                auto var = AbstractSyntax::RefCast<VariableRef>(expr);
                if (variable_exists(var->name));
                    return variables[var->name]; 
                break;
            }
            case SyntaxType::BinaryOp:
            {
                auto op = AbstractSyntax::RefCast<BinaryOp>(expr); 
                if (op->OpType() == BinaryOpType::Comma)
                    return get_value(op->rvalue); 
                break;
            }
            case SyntaxType::Assignment:
            {
                auto assign = AbstractSyntax::RefCast<Assignment>(expr); 
                if (variable_exists(assign->lvalue));
                    return variables[assign->lvalue]; 
                break;
            }
        }
        return std::nullopt; 
    }

    static Expression::Ref FoldConstants(Expression::Ref expr)
    {
        if (expr == nullptr) return expr; 
        switch (expr->type())
        {
            case SyntaxType::AssignmentOp:
            {
                auto op = AbstractSyntax::RefCast<AssignmentOp>(expr); 
                op->rvalue = FoldConstants(op->rvalue); 
                auto rconst = get_value(op->rvalue); 
                if (rconst)
                {
                    auto value = rconst.value(); 
                    if (op->rvalue->type() == SyntaxType::VariableRef)
                        op->rvalue = CreateRef<IntConstant>(value);
                    switch (op->OpType())
                    {
                        case AssignmentOpType::Add:
                            variables[op->lvalue] += value;
                            break;
                        case AssignmentOpType::Minus:
                            variables[op->lvalue] -= value;
                            break;
                        case AssignmentOpType::Multiplication:
                            variables[op->lvalue] *= value;
                            break;
                        case AssignmentOpType::Division:
                            variables[op->lvalue] /= value;
                            break;
                        case AssignmentOpType::Modulo:
                            variables[op->lvalue] %= value; 
                            break;
                        case AssignmentOpType::LeftShift:
                            variables[op->lvalue] <<= value;
                            break;
                        case AssignmentOpType::RightShift:
                            variables[op->lvalue] >>= value;
                            break;
                        case AssignmentOpType::LogicalOr:
                            variables[op->lvalue] |= value;
                            break;
                        case AssignmentOpType::LogicalAnd:
                            variables[op->lvalue] &= value;
                            break;
                        case AssignmentOpType::LogicalXOR:
                            variables[op->lvalue] ^= value;
                            break;
                    }
                } else remove_variable(op->lvalue); 
                break;
            }
            case SyntaxType::UnaryOp:
            {
                auto op = AbstractSyntax::RefCast<UnaryOp>(expr);
                op->expr = FoldConstants(op->expr); 
                auto constant = get_value(op->expr);
                if (constant)
                {
                    switch (op->OpType())
                    {
                        case UnaryOpType::Negation: 
                            return CreateRef<IntConstant>(-constant.value());
                        case UnaryOpType::Complement:
                            return CreateRef<IntConstant>(~constant.value()); 
                        case UnaryOpType::LogicalNegation:
                            return CreateRef<IntConstant>(!constant.value());  
                    }
                }
                break; 
            }
            case SyntaxType::BinaryOp:
            {
                auto op = AbstractSyntax::RefCast<BinaryOp>(expr);
                op->lvalue = FoldConstants(op->lvalue); 
                auto lconst = get_value(op->lvalue); 
                // short-circuit logic
                switch (op->OpType())
                {
                    case BinaryOpType::LogicalOr:
                        if (lconst && lconst.value())
                            return CreateRef<IntConstant>(1); 
                        break;
                    case BinaryOpType::LogicalAnd:
                        if (lconst && !lconst.value())
                            return CreateRef<IntConstant>(0);
                        break;
                }
                op->rvalue = FoldConstants(op->rvalue); 
                auto rconst = get_value(op->rvalue);
                if (lconst && rconst)
                {
                    auto lvalue = lconst.value();
                    auto rvalue = rconst.value(); 
                    switch (op->OpType())
                    {
                        case BinaryOpType::Addition:
                            return CreateRef<IntConstant>(lvalue + rvalue);
                        case BinaryOpType::Subtraction:
                            return CreateRef<IntConstant>(lvalue - rvalue);
                        case BinaryOpType::Multiplication:
                            return CreateRef<IntConstant>(lvalue * rvalue);
                        case BinaryOpType::Division:
                            return CreateRef<IntConstant>(lvalue / rvalue);
                        case BinaryOpType::LogicalOr:
                            return CreateRef<IntConstant>(lvalue || rvalue);
                        case BinaryOpType::LogicalAnd:
                            return CreateRef<IntConstant>(lvalue && rvalue);
                        case BinaryOpType::Equal:
                            return CreateRef<IntConstant>(lvalue == rvalue);
                        case BinaryOpType::NotEqual:
                            return CreateRef<IntConstant>(lvalue != rvalue);
                        case BinaryOpType::Remainder:
                            return CreateRef<IntConstant>(lvalue % rvalue); 
                        case BinaryOpType::LessThan:
                            return CreateRef<IntConstant>(lvalue < rvalue);
                        case BinaryOpType::LessThanOrEqual:
                            return CreateRef<IntConstant>(lvalue <= rvalue);
                        case BinaryOpType::GreaterThan:
                            return CreateRef<IntConstant>(lvalue > rvalue);
                        case BinaryOpType::GreaterThanOrEqual:
                            return CreateRef<IntConstant>(lvalue >= rvalue); 
                        case BinaryOpType::BitwiseOr:
                            return CreateRef<IntConstant>(lvalue | rvalue);
                        case BinaryOpType::BitwiseAnd:
                            return CreateRef<IntConstant>(lvalue & rvalue); 
                        case BinaryOpType::BitwiseXOR:
                            return CreateRef<IntConstant>(lvalue ^ rvalue); 
                        case BinaryOpType::BitwiseLeftShift:
                            return CreateRef<IntConstant>(lvalue << rvalue);
                        case BinaryOpType::BitwiseRightShift:
                            return CreateRef<IntConstant>(lvalue >> rvalue);                             
                    }
                } else if (lconst)
                    return CreateRef<BinaryOp>(op->OpType(), CreateRef<IntConstant>(lconst.value()), op->rvalue); 
                else if (rconst)
                    return CreateRef<BinaryOp>(op->OpType(), op->lvalue, CreateRef<IntConstant>(rconst.value())); 
                break; 
            }
            case SyntaxType::VariableRef:
            {
                auto var = AbstractSyntax::RefCast<VariableRef>(expr);
                if (variable_exists(var->name))
                    return CreateRef<IntConstant>(variables[var->name]);
                break;
            }
            case SyntaxType::Assignment:
            {
                auto assign = AbstractSyntax::RefCast<Assignment>(expr); 
                assign->rvalue = FoldConstants(assign->rvalue); 
                auto constant = get_value(assign->rvalue);
                if (constant) variables[assign->lvalue] = constant.value(); 
                else remove_variable(assign->lvalue); 
                break;
            }
        }
        return expr; 
    }

    // Evaluate unary/binary operations that can be reduced to constants in the abstract syntax tree.
    static AbstractSyntax::Ref EvaluateSyntax(AbstractSyntax::Ref syntax)
    {
        if (syntax == nullptr) return nullptr; 
        switch (syntax->type())
        {
            case SyntaxType::Program:
            {
                auto program = AbstractSyntax::RefCast<Program>(syntax);
                EvaluateSyntax(program->function); 
                break; 
            }
            case SyntaxType::Function:
            {
                auto function = AbstractSyntax::RefCast<Function>(syntax); 
                for (auto statement : function->Statements())
                    EvaluateSyntax(statement); 
                break; 
            }
            case SyntaxType::StatementExpression:
            {
                auto statementExpr = AbstractSyntax::RefCast<StatementExpression>(syntax);
                statementExpr->expr = FoldConstants(statementExpr->expr);
                break;
            }
            case SyntaxType::Declaration:
            {
                auto decl = AbstractSyntax::RefCast<Declaration>(syntax);
                for (auto var : decl->variables)
                {
                    if (var.expr == nullptr) variables[var.name] = 0; 
                    else 
                    {
                        var.expr = FoldConstants(var.expr); 
                        auto constant = get_value(var.expr);
                        if (constant) 
                            variables[var.name] = constant.value(); 
                    }
                }
                break;
            }
            case SyntaxType::Return:
            {
                auto ret = AbstractSyntax::RefCast<Return>(syntax); 
                ret->expr = FoldConstants(ret->expr);
                break; 
            }
        }
        return syntax; 
    }
};

static void OptimizeTree(AbstractSyntax::Ref syntax)
{
    variables.clear(); 
    EvaluateSyntax(syntax); 
}