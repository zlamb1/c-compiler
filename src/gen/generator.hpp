#pragma once

#include <string>

#include "asm/code_gen.hpp"
#include "parser/parser.hpp"

class ASMVisitor : public ASTVisitor
{
    public:
        ASMVisitor(ASMCodeGenerator& codeGenerator) : m_CodeGenerator(codeGenerator)
        {
        }

        void VisitProgram(Program* program) override
        {
            VisitFunction(program->function); 
        }

        void VisitFunction(Function* function) override
        {
            m_CodeGenerator.EmitFun(function->name); 
            // write function body
            m_CodeGenerator.IncreaseIndentation(); 
            function->statement->Accept(this); 
            m_CodeGenerator.DecreaseIndentation();
        }

        void VisitReturn(Return* ret) override
        {
            ret->expr->Accept(this);
            m_CodeGenerator.EmitOp("ret");
        }

        void VisitIntExpr(IntExpr* expr) override
        {
            m_CodeGenerator.EmitOp("mov", ImmediateArg(expr->value), RegisterArg("eax")); 
        }

        void VisitUnaryOp(UnaryOp* op) override
        {
            op->expr->Accept(this); 
            switch (op->type)
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

        void VisitBinaryOp(BinaryOp* op) override
        {
            switch (op->type)
            {
                case BinaryOpType::Addition:
                    op->lvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("push", RegisterArg("rax")); 
                    op->rvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("pop", RegisterArg("rcx"));
                    m_CodeGenerator.EmitOp("add", RegisterArg("ecx"), RegisterArg("eax")); 
                    break;
                case BinaryOpType::Subtraction:
                    op->rvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("push", RegisterArg("rax")); 
                    op->lvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("pop", RegisterArg("rcx"));
                    m_CodeGenerator.EmitOp("sub", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::Multiplication:
                    op->lvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("push", RegisterArg("rax")); 
                    op->rvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("pop", RegisterArg("rcx"));
                    m_CodeGenerator.EmitOp("imul", RegisterArg("ecx"), RegisterArg("eax"));
                    break;
                case BinaryOpType::Division:
                    op->rvalue->Accept(this);
                    m_CodeGenerator.EmitOp("push", RegisterArg("rax")); 
                    op->lvalue->Accept(this); 
                    m_CodeGenerator.EmitOp("cdq");
                    m_CodeGenerator.EmitOp("pop", RegisterArg("rcx"));
                    m_CodeGenerator.EmitOp("idiv", RegisterArg("ecx")); 
                    break; 
                case BinaryOpType::LogicalOr: 
                {
                    auto clause2 = m_CodeGenerator.GenerateLabel();
                    auto end = m_CodeGenerator.GenerateLabel(); 
                    op->lvalue->Accept(this);
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("je", clause2); 
                    m_CodeGenerator.EmitOp("mov", ImmediateArg(1), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("jmp", end);
                    m_CodeGenerator.EmitLabel(clause2);
                    op->rvalue->Accept(this);
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
                    op->lvalue->Accept(this);
                    m_CodeGenerator.EmitOp("cmp", ImmediateArg(0), RegisterArg("eax"));
                    m_CodeGenerator.EmitOp("jne", clause2); 
                    m_CodeGenerator.EmitOp("jmp", end);
                    m_CodeGenerator.EmitLabel(clause2);
                    op->rvalue->Accept(this);
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
            }
        }

    private: 
        ASMCodeGenerator& m_CodeGenerator; 

        void LoadRegisters(Expression* lvalue, Expression* rvalue)
        {
            lvalue->Accept(this); 
            m_CodeGenerator.EmitOp("push", RegisterArg("rax")); 
            rvalue->Accept(this); 
            m_CodeGenerator.EmitOp("pop", RegisterArg("rcx"));
        }
};