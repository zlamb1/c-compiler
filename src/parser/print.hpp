#pragma once

#include <string>
#include <ostream>

#include "syntax/includes.hpp"

class ASTPrinter
{
    public:
        ASTPrinter(std::ostream& outputstream) : m_OutputStream(outputstream)
        {
        }

        void PrintSyntax(AbstractSyntax::Ref syntax)
        {
            switch (syntax->type())
            {
                case SyntaxType::Program: 
                    PrintProgram(AbstractSyntax::RefCast<Program>(syntax));
                    break;
                case SyntaxType::Function:
                    PrintFunction(AbstractSyntax::RefCast<Function>(syntax));
                    break;
                case SyntaxType::StatementExpression:
                    PrintStatementExpression(AbstractSyntax::RefCast<StatementExpression>(syntax));
                    break;
                case SyntaxType::AssignmentOp:
                    PrintAssignmentOp(AbstractSyntax::RefCast<AssignmentOp>(syntax));
                    break;
                case SyntaxType::Declaration:
                    PrintDeclaration(AbstractSyntax::RefCast<Declaration>(syntax));
                    break;
                case SyntaxType::CompoundBlock:
                    PrintCompoundBlock(AbstractSyntax::RefCast<CompoundBlock>(syntax));
                    break;
                case SyntaxType::IfStatement:
                    PrintIfStatement(AbstractSyntax::RefCast<IfStatement>(syntax));
                    break;
                case SyntaxType::Return:
                    PrintReturn(AbstractSyntax::RefCast<Return>(syntax));
                    break;
                case SyntaxType::IntConstant:
                    PrintIntConstant(AbstractSyntax::RefCast<IntConstant>(syntax));
                    break;
                case SyntaxType::UnaryOp:
                    PrintUnaryOp(AbstractSyntax::RefCast<UnaryOp>(syntax));
                    break;
                case SyntaxType::BinaryOp:
                    PrintBinaryOp(AbstractSyntax::RefCast<BinaryOp>(syntax));
                    break;
                case SyntaxType::TernaryOp:
                    PrintTernaryOp(AbstractSyntax::RefCast<TernaryOp>(syntax));
                    break;
                case SyntaxType::VariableRef:
                    PrintVariableRef(AbstractSyntax::RefCast<VariableRef>(syntax));
                    break;
                case SyntaxType::Assignment:
                    PrintAssignment(AbstractSyntax::RefCast<Assignment>(syntax));
                    break;
            }
        }

    private:
        std::ostream& m_OutputStream;

        void PrintProgram(Program::Ref program)
        {
            m_OutputStream << "Program(" << std::endl;
            SetIndent(m_Indent + 1); 
            PrintFunction(program->function);
            SetIndent(m_Indent - 1);
            m_OutputStream << ")" << std::endl;
        }

        void PrintFunction(Function::Ref function)
        {
            m_OutputStream << m_Spaces << function->name << "(" << std::endl;
            SetIndent(m_Indent + 1); 
            for (auto statement : function->Statements())
                PrintSyntax(statement);
            SetIndent(m_Indent - 1); 
            m_OutputStream << m_Spaces << ")" << std::endl;
        }

        void PrintStatementExpression(StatementExpression::Ref statementExpr)
        {
            if (statementExpr->expr)
            {
                m_OutputStream << m_Spaces;
                PrintSyntax(statementExpr->expr);
                m_OutputStream << ";\n";
            } 
            else m_OutputStream << m_Spaces << ";" << std::endl;
        }

        void PrintAssignmentOp(AssignmentOp::Ref op)
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

        void PrintDeclaration(Declaration::Ref decl)
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

        void PrintCompoundBlock(CompoundBlock::Ref compoundBlock)
        {
            m_OutputStream << m_Spaces << "{\n";
            SetIndent(m_Indent + 1); 
            if (compoundBlock->statements.empty()) 
                m_OutputStream << m_Spaces << "EMPTY BLOCK\n";
            for (auto statement : compoundBlock->statements)
                PrintSyntax(statement); 
            SetIndent(m_Indent - 1); 
            m_OutputStream << m_Spaces << "}\n";
        }

        void PrintBlockItem(Statement::Ref statement)
        {
            if (statement->type() != SyntaxType::CompoundBlock)
                std::cout << m_Spaces;
            PrintSyntax(statement); 
        }

        void PrintIfStatement(IfStatement::Ref ifStatement)
        {
            m_OutputStream << m_Spaces << "IF (";
            PrintSyntax(ifStatement->if_conditional.condition); 
            m_OutputStream << ")\n";
            PrintBlockItem(ifStatement->if_conditional.statement);
            for (auto else_if : ifStatement->else_ifs)
            {
                m_OutputStream << m_Spaces << "ELSE IF (";
                PrintSyntax(else_if.condition); 
                m_OutputStream << ")\n"; 
                PrintBlockItem(else_if.statement); 
            }
            if (ifStatement->else_statement != nullptr)
            {
                m_OutputStream << m_Spaces << "ELSE\n";
                PrintBlockItem(ifStatement->else_statement); 
            }
        }

        void PrintReturn(Return::Ref ret)
        {
            m_OutputStream << m_Spaces << "Return(";
            SetIndent(m_Indent + 1);  
            PrintSyntax(ret->expr);
            SetIndent(m_Indent - 1); 
            m_OutputStream << ");\n";
        }

        void PrintIntConstant(IntConstant::Ref constant)
        {
            m_OutputStream << "IntConstant(" << constant->value << ")";
        }

        void PrintUnaryOp(UnaryOp::Ref op)
        {
            switch (op->OpType())
            {
                case UnaryOpType::Negation:         m_OutputStream << "-"; break;
                case UnaryOpType::Complement:       m_OutputStream << "~"; break;
                case UnaryOpType::LogicalNegation:  m_OutputStream << "!"; break;
                case UnaryOpType::PrefixDecrement:  m_OutputStream << "--"; break;
                case UnaryOpType::PrefixIncrement:  m_OutputStream << "++"; break;
                case UnaryOpType::PostfixDecrement:
                    PrintSyntax(op->expr);
                    m_OutputStream << "--";
                    return;
                case UnaryOpType::PostfixIncrement:
                    PrintSyntax(op->expr);
                    m_OutputStream << "++";
                    return;
            }
            PrintSyntax(op->expr);
        }

        void PrintBinaryOp(BinaryOp::Ref op)
        {
            m_OutputStream << "(";
            PrintSyntax(op->lvalue);
            switch (op->OpType())
            {
                case BinaryOpType::Addition:           m_OutputStream << "+";  break;
                case BinaryOpType::Subtraction:        m_OutputStream << "-";  break;
                case BinaryOpType::Multiplication:     m_OutputStream << "*";  break;
                case BinaryOpType::Division:           m_OutputStream << "/";  break;
                case BinaryOpType::LogicalOr:          m_OutputStream << "||"; break;
                case BinaryOpType::LogicalAnd:         m_OutputStream << "&&"; break;
                case BinaryOpType::Equal:              m_OutputStream << "=="; break;
                case BinaryOpType::NotEqual:           m_OutputStream << "!="; break;
                case BinaryOpType::LessThan:           m_OutputStream << "<";  break;
                case BinaryOpType::LessThanOrEqual:    m_OutputStream << "<="; break;
                case BinaryOpType::GreaterThan:        m_OutputStream << ">";  break;
                case BinaryOpType::GreaterThanOrEqual: m_OutputStream << ">="; break;
                case BinaryOpType::Remainder:          m_OutputStream << "%";  break; 
                case BinaryOpType::BitwiseOr:          m_OutputStream << "|";  break;
                case BinaryOpType::BitwiseAnd:         m_OutputStream << "&";  break;
                case BinaryOpType::BitwiseXOR:         m_OutputStream << "^";  break;
                case BinaryOpType::BitwiseLeftShift:   m_OutputStream << "<<"; break;
                case BinaryOpType::BitwiseRightShift:  m_OutputStream << ">>"; break; 
                case BinaryOpType::Comma:              m_OutputStream << ",";  break;
            }
            PrintSyntax(op->rvalue);
            m_OutputStream << ")";
        }

        void PrintTernaryOp(TernaryOp::Ref op)
        {
            PrintSyntax(op->condition);
            m_OutputStream << " ? ";
            PrintSyntax(op->lvalue);
            m_OutputStream << " : ";
            PrintSyntax(op->rvalue);
        }

        void PrintVariableRef(VariableRef::Ref ref)
        {
            m_OutputStream << ref->name; 
        }

        void PrintAssignment(Assignment::Ref assignment)
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

static void PrettyPrintAST(AbstractSyntax::Ref ast)
{
    if (ast != nullptr)
    {
        ASTPrinter printer(std::cout); 
        printer.PrintSyntax(ast); 
    } else std::cout << "The abstract syntax tree is null!" << std::endl; 
}