#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "asm/code_gen.hpp"
#include "exc.hpp"
#include "parser/parser.hpp"

class BlockContext
{
public:
    int CreateVariable(const std::string& variableName, int offset)
    {
        auto index = m_StackIndex;
        m_VariableMap[variableName] = index;
        m_StackIndex -= offset;  
        return index;
    }

    bool HasVariable(const std::string& variableName)
    {
        return m_VariableMap.find(variableName) != m_VariableMap.end(); 
    }

    int GetStackOffset(const std::string& variableName)
    {
        if (m_VariableMap.find(variableName) == m_VariableMap.end())
        {
            // TODO: handle 
        }
        return m_VariableMap[variableName]; 
    }

    int& StackOffset() { return m_StackIndex; }

private:
    // variable name : stack index
    std::unordered_map<std::string, int> m_VariableMap; 
    int m_StackIndex = 0;
};

class ASMGenerator
{
    public:
        ASMGenerator(ASMCodeGenerator& codeGenerator) : m_CodeGenerator(codeGenerator)
        {
        }

        void GenerateSyntax(AbstractSyntax* syntax)
        {
            switch (syntax->type())
            {
                case SyntaxType::Program:
                    GenerateProgram(dynamic_cast<Program*>(syntax)); 
                    break;
                case SyntaxType::Function:
                    GenerateFunction(dynamic_cast<Function*>(syntax));
                    break; 
                case SyntaxType::StatementExpression:
                    GenerateStatementExpression(dynamic_cast<StatementExpression*>(syntax));
                    break;
                case SyntaxType::AssignmentOp:
                    GenerateAssignmentOp(dynamic_cast<AssignmentOp*>(syntax));
                    break;
                case SyntaxType::Declaration:
                    GenerateDeclaration(dynamic_cast<Declaration*>(syntax));
                    break;
                case SyntaxType::Return:
                    GenerateReturn(dynamic_cast<Return*>(syntax)); 
                    break; 
                case SyntaxType::IntConstant:
                    GenerateIntConstant(dynamic_cast<IntConstant*>(syntax)); 
                    break;
                case SyntaxType::UnaryOp:
                    GenerateUnaryOp(dynamic_cast<UnaryOp*>(syntax)); 
                    break;
                case SyntaxType::BinaryOp:
                    GenerateBinaryOp(dynamic_cast<BinaryOp*>(syntax)); 
                    break; 
                case SyntaxType::VariableRef:
                    GenerateVariableRef(dynamic_cast<VariableRef*>(syntax));
                    break;
                case SyntaxType::Assignment:
                    GenerateAssignment(dynamic_cast<Assignment*>(syntax));
                    break;
            }
        }

        void GenerateProgram(Program* program)
        {
            GenerateFunction(program->function); 
        }

        void GenerateFunction(Function* function)
        {
            m_ContextStack.push(BlockContext()); 
            auto& context = m_ContextStack.top(); 
            m_CodeGenerator.EmitFun(function->name); 
            m_CodeGenerator.IncreaseIndentation(); 
            // generate function prologue
            m_CodeGenerator.EmitOp("pushq", RegisterArg("rbp"));
            m_CodeGenerator.EmitOp("movq", RegisterArg("rsp"), RegisterArg("rbp"));
            context.StackOffset() -= 8; 
            // generate function body
            if (function->statements.size() == 0)
                GenerateSyntax(new Return(new IntConstant(0))); 
            bool foundReturn = false; 
            for (size_t i = 0; i < function->statements.size(); i++)
            {
                auto statement = function->statements[i]; 
                GenerateSyntax(statement);
                if (statement->type() == SyntaxType::Return)
                {
                    if (foundReturn)
                    {
                        // TODO: error
                    } else foundReturn = true; 
                }
                // generate return statement if one does not exist
                if (i == function->statements.size() - 1 && !foundReturn)
                    GenerateSyntax(new Return(new IntConstant(0))); 
            }
            m_CodeGenerator.DecreaseIndentation();
        }

        void GenerateStatementExpression(StatementExpression* statementExpr)
        {
            if (statementExpr->expr)
                GenerateSyntax(statementExpr->expr); 
        }

        void GenerateAssignmentOp(AssignmentOp* op)
        {
            auto& context = m_ContextStack.top(); 
            if (!context.HasVariable(op->lvalue))
                throw UndeclaredException(op->lvalue);
            auto offset = context.GetStackOffset(op->lvalue); 
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

        void GenerateDeclaration(Declaration* decl)
        {
            auto& context = m_ContextStack.top(); 
            for (auto var : decl->variables)
            {
                if (context.HasVariable(var.name))
                    throw RedeclaredException(var.name);
                auto offset = context.CreateVariable(var.name, 4); 
                if (var.expr != nullptr) GenerateSyntax(var.expr); 
                else m_CodeGenerator.EmitOp("movl", ImmediateArg(0), RegisterArg("eax")); 
                m_CodeGenerator.EmitOp("movl", RegisterArg("eax"), OffsetArg("rbp", offset)); 
            }
        }

        void GenerateReturn(Return* ret)
        {
            if (ret->expr)
                GenerateSyntax(ret->expr); 
            // generate function epilogue
            m_CodeGenerator.EmitOp("movq", RegisterArg("rbp"), RegisterArg("rsp")); 
            m_CodeGenerator.EmitOp("popq", RegisterArg("rbp"));
            m_CodeGenerator.EmitOp("ret");
        }

        void GenerateIntConstant(IntConstant* expr)
        {
            m_CodeGenerator.EmitOp("movl", ImmediateArg(expr->value), RegisterArg(m_DestinationRegister)); 
        }

        void GenerateUnaryOp(UnaryOp* op)
        {
            GenerateSyntax(op->expr); 
            switch (op->opType)
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

        void GenerateBinaryOp(BinaryOp* op)
        {
            switch (op->opType)
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

        void GenerateVariableRef(VariableRef* ref)
        {
            auto& context = m_ContextStack.top();
            if (!context.HasVariable(ref->name))
                throw UndeclaredException(ref->name); 
            auto offset = context.GetStackOffset(ref->name); 
            m_CodeGenerator.EmitOp("movl", OffsetArg("rbp", offset), RegisterArg(m_DestinationRegister)); 
        }

        void GenerateAssignment(Assignment* assignment)
        {
            auto& context = m_ContextStack.top(); 
            if (!context.HasVariable(assignment->lvalue))
                throw UndeclaredException(assignment->lvalue);
            GenerateSyntax(assignment->rvalue); 
            auto offset = context.GetStackOffset(assignment->lvalue);
            m_CodeGenerator.EmitOp("movl", RegisterArg("eax"), OffsetArg("rbp", offset)); 
        }

    private: 
        ASMCodeGenerator& m_CodeGenerator; 
        std::stack<BlockContext> m_ContextStack;
        std::string m_DestinationRegister = "eax"; 

        void ResetDestination()
        {
            m_DestinationRegister = "eax"; 
        }

        void SetDestination(const std::string& newDestination)
        {
            m_DestinationRegister = newDestination; 
        }

        void LoadRegisters(Expression* lvalue, Expression* rvalue)
        {
            SetDestination("eax");
            GenerateSyntax(lvalue);
            SetDestination("ecx"); 
            GenerateSyntax(rvalue); 
            ResetDestination(); 
        }
};