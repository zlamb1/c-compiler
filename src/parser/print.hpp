#pragma once

#include <string>
#include <ostream>

#include "syntax/as.hpp"
#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/expr.hpp"
#include "syntax/assignment_op.hpp"

namespace
{
    template<typename T>
    T* down_cast(AbstractSyntax* ptr)
    {
        return dynamic_cast<T*>(ptr); 
    }
};

class ASTPrinter
{
    public:
        ASTPrinter(std::ostream& outputstream) : m_OutputStream(outputstream)
        {
        }

        void PrintSyntax(AbstractSyntax* syntax)
        {
            switch (syntax->type())
            {
                case SyntaxType::Program: 
                    PrintProgram(down_cast<Program>(syntax));
                    break;
                case SyntaxType::Function:
                    PrintFunction(down_cast<Function>(syntax));
                    break;
                case SyntaxType::StatementExpression:
                    PrintStatementExpression(down_cast<StatementExpression>(syntax));
                    break;
                case SyntaxType::AssignmentOp:
                    PrintAssignmentOp(down_cast<AssignmentOp>(syntax));
                    break;
                case SyntaxType::Declaration:
                    PrintDeclaration(down_cast<Declaration>(syntax));
                    break;
                case SyntaxType::Return:
                    PrintReturn(down_cast<Return>(syntax));
                    break;
                case SyntaxType::IntConstant:
                    PrintIntConstant(down_cast<IntConstant>(syntax));
                    break;
                case SyntaxType::UnaryOp:
                    PrintUnaryOp(down_cast<UnaryOp>(syntax));
                    break;
                case SyntaxType::BinaryOp:
                    PrintBinaryOp(down_cast<BinaryOp>(syntax));
                    break;
                case SyntaxType::VariableRef:
                    PrintVariableRef(down_cast<VariableRef>(syntax));
                    break;
                case SyntaxType::Assignment:
                    PrintAssignment(down_cast<Assignment>(syntax));
                    break;
            }
        }

    private:
        std::ostream& m_OutputStream;

        void PrintProgram(Program* program)
        {
            m_OutputStream << "Program(" << std::endl;
            SetIndent(m_Indent + 1); 
            PrintFunction(program->function);
            SetIndent(m_Indent - 1);
            m_OutputStream << ")" << std::endl;
        }

        void PrintFunction(Function* function)
        {
            m_OutputStream << m_Spaces << function->name << "(" << std::endl;
            SetIndent(m_Indent + 1); 
            for (auto statement : function->statements)
                PrintSyntax(statement);
            SetIndent(m_Indent - 1); 
            m_OutputStream << m_Spaces << ")" << std::endl;
        }

        void PrintStatementExpression(StatementExpression* statementExpr)
        {
            if (statementExpr->expr)
            {
                m_OutputStream << m_Spaces;
                PrintSyntax(statementExpr->expr);
                m_OutputStream << ";\n";
            } 
            else m_OutputStream << m_Spaces << "NULL STATEMENT;" << std::endl;
        }

        void PrintAssignmentOp(AssignmentOp* op)
        {
            m_OutputStream << op->lvalue << " "; 
            switch (op->OpType())
            {
                case AssignmentOpType::Add:            m_OutputStream << "+="; break; 
                case AssignmentOpType::Minus:          m_OutputStream << "-="; break;
                case AssignmentOpType::Multiplication: m_OutputStream << "*="; break;
                case AssignmentOpType::Division:       m_OutputStream << "/="; break; 
                case AssignmentOpType::Modulo:         m_OutputStream << "%="; break; 
                case AssignmentOpType::LeftShift:      m_OutputStream << "<<="; break;
                case AssignmentOpType::RightShift:     m_OutputStream << ">>="; break;
                case AssignmentOpType::LogicalOr:      m_OutputStream << "|="; break;
                case AssignmentOpType::LogicalAnd:     m_OutputStream << "&="; break;
                case AssignmentOpType::LogicalXOR:     m_OutputStream << "^="; break; 
            }
            m_OutputStream << " ";
            PrintSyntax(op->rvalue);
        }

        void PrintDeclaration(Declaration* decl)
        {
            size_t num_vars = decl->variables.size(); 
            m_OutputStream << m_Spaces << "int "; 
            for (size_t i = 0; i < num_vars; i++)
            {
                auto var = decl->variables[i]; 
                m_OutputStream << var.name; 
                if (var.expr)
                {
                    m_OutputStream << " = ";
                    PrintSyntax(var.expr);
                }
                if (i != num_vars - 1) m_OutputStream << ", ";
            }
            m_OutputStream << ";\n"; 
        }

        void PrintReturn(Return* ret)
        {
            m_OutputStream << m_Spaces << "Return(" << std::endl;
            SetIndent(m_Indent + 1);  
            m_OutputStream << m_Spaces; 
            PrintSyntax(ret->expr);
            m_OutputStream << "\n";
            SetIndent(m_Indent - 1); 
            m_OutputStream << m_Spaces << ");" << std::endl;
        }

        void PrintIntConstant(IntConstant* constant)
        {
            m_OutputStream << "IntConstant(" << constant->value << ")";
        }

        void PrintUnaryOp(UnaryOp* op)
        {
            switch (op->opType)
            {
                case UnaryOpType::Negation:        m_OutputStream << "-"; break;
                case UnaryOpType::Complement:      m_OutputStream << "~"; break;
                case UnaryOpType::LogicalNegation: m_OutputStream << "!"; break;
            }
            PrintSyntax(op->expr);
        }

        void PrintBinaryOp(BinaryOp* op)
        {
            m_OutputStream << "(";
            PrintSyntax(op->lvalue);
            switch (op->opType)
            {
                case BinaryOpType::Addition:           m_OutputStream << "+"; break;
                case BinaryOpType::Subtraction:        m_OutputStream << "-"; break;
                case BinaryOpType::Multiplication:     m_OutputStream << "*"; break;
                case BinaryOpType::Division:           m_OutputStream << "/"; break;
                case BinaryOpType::LogicalOr:          m_OutputStream << "||"; break;
                case BinaryOpType::LogicalAnd:         m_OutputStream << "&&"; break;
                case BinaryOpType::Equal:              m_OutputStream << "=="; break;
                case BinaryOpType::NotEqual:           m_OutputStream << "!="; break;
                case BinaryOpType::LessThan:           m_OutputStream << "<"; break;
                case BinaryOpType::LessThanOrEqual:    m_OutputStream << "<="; break;
                case BinaryOpType::GreaterThan:        m_OutputStream << ">"; break;
                case BinaryOpType::GreaterThanOrEqual: m_OutputStream << ">="; break;
                case BinaryOpType::Remainder:          m_OutputStream << "%"; break; 
                case BinaryOpType::BitwiseOr:          m_OutputStream << "|"; break;
                case BinaryOpType::BitwiseAnd:         m_OutputStream << "&"; break;
                case BinaryOpType::BitwiseXOR:         m_OutputStream << "^"; break;
                case BinaryOpType::BitwiseLeftShift:   m_OutputStream << "<<"; break;
                case BinaryOpType::BitwiseRightShift:  m_OutputStream << ">>"; break; 
                case BinaryOpType::Comma:              m_OutputStream << ","; break;
            }
            PrintSyntax(op->rvalue);
            m_OutputStream << ")";
        }

        void PrintVariableRef(VariableRef* ref)
        {
            m_OutputStream << ref->name; 
        }

        void PrintAssignment(Assignment* assignment)
        {
            m_OutputStream << assignment->lvalue << " = ";
            PrintSyntax(assignment->rvalue);
        }

    private:
        size_t m_Indent = 0; 
        size_t m_IndentSize = 2; 

        std::string m_Spaces; 
        void SetIndent(size_t indent)
        {
            m_Indent = indent;
            m_Spaces = std::string(m_Indent * m_IndentSize, ' '); 
        }
};

static void PrettyPrintAST(AbstractSyntax* ast)
{
    if (ast != nullptr)
    {
        ASTPrinter printer(std::cout); 
        printer.PrintSyntax(ast); 
    } else std::cout << "The abstract syntax tree is null!" << std::endl; 
}