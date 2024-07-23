#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "asm/code_gen.hpp"
#include "context.hpp"
#include "exc.hpp"
#include "parser/parser.hpp"

class ASMGenerator
{
    public:
        ASMGenerator(ASMCodeGenerator& codeGenerator) : m_CodeGenerator(codeGenerator)
        {
        }

        void GenerateSyntax(AbstractSyntax::Ref syntax)
        {
            switch (syntax->type())
            {
                case SyntaxType::Program:
                    GenerateProgram(AbstractSyntax::RefCast<Program>(syntax)); 
                    break;
                case SyntaxType::Function:
                    GenerateFunction(AbstractSyntax::RefCast<Function>(syntax));
                    break; 
                case SyntaxType::StatementExpression:
                    GenerateStatementExpression(AbstractSyntax::RefCast<StatementExpression>(syntax));
                    break;
                case SyntaxType::AssignmentOp:
                    GenerateAssignmentOp(AbstractSyntax::RefCast<AssignmentOp>(syntax));
                    break;
                case SyntaxType::Declaration:
                    GenerateDeclaration(AbstractSyntax::RefCast<Declaration>(syntax));
                    break;
                case SyntaxType::CompoundBlock:
                    GenerateCompoundBlock(AbstractSyntax::RefCast<CompoundBlock>(syntax));
                    break;
                case SyntaxType::IfStatement:
                    GenerateIfStatement(AbstractSyntax::RefCast<IfStatement>(syntax));
                    break; 
                case SyntaxType::Return:
                    GenerateReturn(AbstractSyntax::RefCast<Return>(syntax)); 
                    break; 
                case SyntaxType::IntConstant:
                    GenerateIntConstant(AbstractSyntax::RefCast<IntConstant>(syntax)); 
                    break;
                case SyntaxType::UnaryOp:
                    GenerateUnaryOp(AbstractSyntax::RefCast<UnaryOp>(syntax)); 
                    break;
                case SyntaxType::BinaryOp:
                    GenerateBinaryOp(AbstractSyntax::RefCast<BinaryOp>(syntax)); 
                    break; 
                case SyntaxType::VariableRef:
                    GenerateVariableRef(AbstractSyntax::RefCast<VariableRef>(syntax));
                    break;
                case SyntaxType::Assignment:
                    GenerateAssignment(AbstractSyntax::RefCast<Assignment>(syntax));
                    break;
            }
        }

        void GenerateProgram(Program::Ref program)
        {
            GenerateFunction(program->function); 
        }

        void GenerateFunction(Function::Ref function)
        {
            m_ContextStack.push(CreateRef<BlockContext>()); 
            auto& context = m_ContextStack.top(); 
            m_CodeGenerator.EmitFun(function->name); 
            m_CodeGenerator.IncreaseIndentation(); 
            // generate function prologue
            m_CodeGenerator.EmitOp("pushq", RegisterArg("rbp"));
            m_CodeGenerator.EmitOp("movq", RegisterArg("rsp"), RegisterArg("rbp"));
            context->StackOffset() -= 8; 
            // generate function body
            if (function->Statements().size() == 0)
                GenerateSyntax(CreateRef<Return>(CreateRef<IntConstant>(0))); 
            bool foundReturn = false; 
            for (size_t i = 0; i < function->Statements().size(); i++)
            {
                auto statement = function->Statements()[i]; 
                GenerateSyntax(statement);
                if (statement->type() == SyntaxType::Return)
                {
                    if (foundReturn)
                    {
                        // TODO: error
                    } else foundReturn = true; 
                }
                // generate return statement if one does not exist
                if (i == function->Statements().size() - 1 && !foundReturn)
                    GenerateSyntax(CreateRef<Return>(CreateRef<IntConstant>(0))); 
            }
            m_CodeGenerator.DecreaseIndentation();
        }

        void GenerateStatementExpression(StatementExpression::Ref statementExpr)
        {
            if (statementExpr->expr)
                GenerateSyntax(statementExpr->expr); 
        }

        void GenerateAssignmentOp(AssignmentOp::Ref op)
        {
            auto& context = m_ContextStack.top(); 
            if (!context->HasVariable(op->lvalue))
                throw UndeclaredException(op->lvalue);
            auto offset = context->GetVariableOffset(op->lvalue); 
            m_CodeGenerator.EmitOp("movl", OffsetArg("rbp", offset), RegisterArg("eax"));
            SetDestination("ecx"); 
            GenerateSyntax(op->rvalue); 
            ResetDestination(); 
            switch (op->OpType())
            {
                case AssignmentOpType::Add:
                    m_CodeGenerator.EmitOp("addl", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::Minus:
                    m_CodeGenerator.EmitOp("subl", RegisterArg("ecx"), RegisterArg("eax")); 
                    break; 
                case AssignmentOpType::Multiplication:
                    m_CodeGenerator.EmitOp("imul", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::Division:
                    m_CodeGenerator.EmitOp("cdq"); 
                    m_CodeGenerator.EmitOp("idiv", RegisterArg("ecx"));
                    break;
                case AssignmentOpType::Modulo:
                    m_CodeGenerator.EmitOp("cdq");
                    m_CodeGenerator.EmitOp("idiv", RegisterArg("ecx")); 
                    m_CodeGenerator.EmitOp("movl", RegisterArg("edx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::LeftShift:
                    m_CodeGenerator.EmitOp("sal", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case AssignmentOpType::RightShift:
                    m_CodeGenerator.EmitOp("sar", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::LogicalOr:
                    m_CodeGenerator.EmitOp("or", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::LogicalAnd:
                    m_CodeGenerator.EmitOp("and", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case AssignmentOpType::LogicalXOR:
                    m_CodeGenerator.EmitOp("xor", RegisterArg("ecx"), RegisterArg("eax")); 
                    break; 
            }
            m_CodeGenerator.EmitOp("movl", RegisterArg("eax"), OffsetArg("rbp", offset));
        }

        void GenerateDeclaration(Declaration::Ref decl)
        {
            auto& context = m_ContextStack.top(); 
            for (auto var : decl->variables)
            {
                if (context->HasScopeVariable(var.name))
                    throw RedeclaredException(var.name);
                auto offset = context->CreateVariable(var.name, 4); 
                if (var.expr != nullptr) GenerateSyntax(var.expr); 
                else m_CodeGenerator.EmitOp("movl", ImmediateArg(0), RegisterArg("eax")); 
                m_CodeGenerator.EmitOp("movl", RegisterArg("eax"), OffsetArg("rbp", offset)); 
            }
        }

        void GenerateCompoundBlock(CompoundBlock::Ref compoundBlock)
        {
            m_ContextStack.push(CreateRef<BlockContext>(m_ContextStack.top()));
            for (auto statement : compoundBlock->statements)
                GenerateSyntax(statement);
            m_ContextStack.pop(); 
        }

        void GenerateIfStatement(IfStatement::Ref ifStatement)
        {
            bool has_else = ifStatement->else_statement != nullptr; 
            auto end = m_CodeGenerator.GenerateLabel();
            std::vector<LabelArg> labels;
            for (auto else_if : ifStatement->else_ifs)
                labels.emplace_back(m_CodeGenerator.GenerateLabel());
            if (has_else)
                labels.emplace_back(m_CodeGenerator.GenerateLabel()); 
            GenerateSyntax(ifStatement->if_conditional.condition);
            m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax")); 
            m_CodeGenerator.EmitOp("je", labels.empty() ? end : labels[0]);
            GenerateSyntax(ifStatement->if_conditional.statement);
            if (!labels.empty())
                m_CodeGenerator.EmitOp("jmp", end);
            size_t len = ifStatement->else_ifs.size();
            for (size_t i = 0; i < len; i++)
            {
                auto& else_if = ifStatement->else_ifs[i];
                m_CodeGenerator.EmitLabel(labels[i]);
                GenerateSyntax(else_if.condition);
                bool last = labels.size() == len && i == len - 1;
                if (!last)
                {
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax")); 
                    m_CodeGenerator.EmitOp("je", i < labels.size() - 1 ? labels[i + 1] : end);
                }
                GenerateSyntax(else_if.statement); 
                if (!last) m_CodeGenerator.EmitOp("jmp", end);
            }
            if (has_else)    
            {
                m_CodeGenerator.EmitLabel(labels[labels.size() - 1]);       
                GenerateSyntax(ifStatement->else_statement);
            }
            m_CodeGenerator.EmitLabel(end); 
        }

        void GenerateReturn(Return::Ref ret)
        {
            if (ret->expr)
                GenerateSyntax(ret->expr); 
            // generate function epilogue
            m_CodeGenerator.EmitOp("movq", RegisterArg("rbp"), RegisterArg("rsp")); 
            m_CodeGenerator.EmitOp("popq", RegisterArg("rbp"));
            m_CodeGenerator.EmitOp("ret");
        }

        void GenerateIntConstant(IntConstant::Ref expr)
        {
            m_CodeGenerator.EmitOp("movl", ImmediateArg(expr->value), RegisterArg(m_DestinationRegister)); 
        }

        void GenerateUnaryOp(UnaryOp::Ref op)
        {
            GenerateSyntax(op->expr); 
            switch (op->OpType())
            {
                case UnaryOpType::Negation:
                    m_CodeGenerator.EmitOp("neg", RegisterArg("eax")); 
                    break;
                case UnaryOpType::Complement:
                    m_CodeGenerator.EmitOp("not", RegisterArg("eax"));
                    break;
                case UnaryOpType::LogicalNegation:
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("sete", RegisterArg("al")); 
                    break;
            }
        }

        void GenerateBinaryOp(BinaryOp::Ref op)
        {
            switch (op->OpType())
            {
                case BinaryOpType::Addition:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("add", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case BinaryOpType::Subtraction:
                    LoadRegisters(op->rvalue, op->lvalue); 
                    m_CodeGenerator.EmitOp("sub", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::Multiplication:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("imul", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::Division:
                    LoadRegisters(op->rvalue, op->lvalue); 
                    m_CodeGenerator.EmitOp("cdq");
                    m_CodeGenerator.EmitOp("idiv", RegisterArg("ecx")); 
                    break; 
                case BinaryOpType::LogicalOr: 
                {
                    auto clause2 = m_CodeGenerator.GenerateLabel();
                    auto end = m_CodeGenerator.GenerateLabel(); 
                    GenerateSyntax(op->lvalue); 
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("je", clause2); 
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(1), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("jmp", end);
                    m_CodeGenerator.EmitLabel(clause2);
                    GenerateSyntax(op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax")); 
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax")); 
                    m_CodeGenerator.EmitOp("setne", RegisterArg("al")); 
                    m_CodeGenerator.EmitLabel(end);
                    break;
                }
                case BinaryOpType::LogicalAnd: 
                {
                    auto clause2 = m_CodeGenerator.GenerateLabel();
                    auto end = m_CodeGenerator.GenerateLabel(); 
                    GenerateSyntax(op->lvalue); 
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("jne", clause2); 
                    m_CodeGenerator.EmitOp("jmp", end);
                    m_CodeGenerator.EmitLabel(clause2);
                    GenerateSyntax(op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax")); 
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax")); 
                    m_CodeGenerator.EmitOp("setne", RegisterArg("al")); 
                    m_CodeGenerator.EmitLabel(end);
                    break;
                }
                case BinaryOpType::Equal:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("sete", RegisterArg("al")); 
                    break;
                case BinaryOpType::NotEqual:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("setne", RegisterArg("al")); 
                    break;
                case BinaryOpType::LessThan:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("setl", RegisterArg("al")); 
                    break;
                case BinaryOpType::LessThanOrEqual:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("setle", RegisterArg("al")); 
                    break;
                case BinaryOpType::GreaterThan:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("setg", RegisterArg("al")); 
                    break;
                case BinaryOpType::GreaterThanOrEqual:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("cmp", RegisterArg("eax"), RegisterArg("ecx"));
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("setge", RegisterArg("al")); 
                    break;
                case BinaryOpType::Remainder:
                    LoadRegisters(op->rvalue, op->lvalue);
                    m_CodeGenerator.EmitOp("cdq");
                    m_CodeGenerator.EmitOp("idiv", RegisterArg("ecx")); 
                    m_CodeGenerator.EmitOp("mov", RegisterArg("edx"), RegisterArg("eax")); 
                    break; 
                case BinaryOpType::BitwiseOr:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("or", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case BinaryOpType::BitwiseAnd:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("and", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case BinaryOpType::BitwiseXOR:
                    LoadRegisters(op->lvalue, op->rvalue); 
                    m_CodeGenerator.EmitOp("xor", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case BinaryOpType::BitwiseLeftShift:
                    LoadRegisters(op->rvalue, op->lvalue); 
                    m_CodeGenerator.EmitOp("sal", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::BitwiseRightShift:
                    LoadRegisters(op->rvalue, op->lvalue); 
                    m_CodeGenerator.EmitOp("sar", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::Comma:
                    // evaluate both operands
                    GenerateSyntax(op->lvalue);
                    GenerateSyntax(op->rvalue); 
                    break;
            }
        }

        void GenerateVariableRef(VariableRef::Ref ref)
        {
            auto& context = m_ContextStack.top();
            if (!context->HasVariable(ref->name))
                throw UndeclaredException(ref->name); 
            auto offset = context->GetVariableOffset(ref->name); 
            m_CodeGenerator.EmitOp("movl", OffsetArg("rbp", offset), RegisterArg(m_DestinationRegister)); 
        }

        void GenerateAssignment(Assignment::Ref assignment)
        {
            auto& context = m_ContextStack.top(); 
            if (!context->HasVariable(assignment->lvalue))
                throw UndeclaredException(assignment->lvalue);
            GenerateSyntax(assignment->rvalue); 
            auto offset = context->GetVariableOffset(assignment->lvalue);
            m_CodeGenerator.EmitOp("movl", RegisterArg("eax"), OffsetArg("rbp", offset)); 
        }

    private: 
        ASMCodeGenerator& m_CodeGenerator; 
        std::stack<BlockContext::Ref> m_ContextStack;
        std::string m_DestinationRegister = "eax"; 

        void ResetDestination()
        {
            m_DestinationRegister = "eax"; 
        }

        void SetDestination(const std::string& newDestination)
        {
            m_DestinationRegister = newDestination; 
        }

        void LoadRegisters(Expression::Ref lvalue, Expression::Ref rvalue)
        {
            SetDestination("eax");
            GenerateSyntax(lvalue);
            SetDestination("ecx"); 
            GenerateSyntax(rvalue); 
            ResetDestination(); 
        }
};