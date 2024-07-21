#pragma once

#include <string>

#include "syntax/as.hpp"
#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/expr.hpp"

class ASTPrinter : public ASTVisitor
{
    public:
        void VisitProgram(Program* program) override
        {
            std::cout << "Program(" << std::endl;
            SetIndent(m_Indent + 1); 
            VisitFunction(program->function);
            SetIndent(m_Indent - 1);
            std::cout << ")" << std::endl;
        }

        void VisitFunction(Function* function) override
        {
            std::cout << m_Spaces << function->name << "(" << std::endl;
            SetIndent(m_Indent + 1); 
            for (auto statement : function->statements)
                statement->Accept(this);
            SetIndent(m_Indent - 1); 
            std::cout << m_Spaces << ")" << std::endl;
        }

        void VisitStatementExpression(StatementExpression* statementExpr) override
        {
            if (statementExpr->expr)
            {
                std::cout << m_Spaces;
                statementExpr->expr->Accept(this);
                std::cout << ";\n";
            } 
            else std::cout << m_Spaces << "NULL STATEMENT;" << std::endl;
        }

        void VisitDeclaration(Declaration* decl) override
        {
            if (decl->expr)
            {
                std::cout << m_Spaces << decl->name << " = ";
                decl->expr->Accept(this); 
                std::cout << ";\n";
            }
            else std::cout << m_Spaces << decl->name << ";" << std::endl;
        }

        void VisitReturn(Return* ret) override
        {
            std::cout << m_Spaces << "Return(" << std::endl;
            SetIndent(m_Indent + 1);  
            std::cout << m_Spaces; 
            ret->expr->Accept(this); 
            std::cout << "\n";
            SetIndent(m_Indent - 1); 
            std::cout << m_Spaces << ");" << std::endl;
        }

        void VisitIntConstant(IntConstant* constant) override
        {
            std::cout << "IntConstant(" << constant->value << ")";
        }

        void VisitUnaryOp(UnaryOp* op) override
        {
            switch (op->opType)
            {
                case UnaryOpType::Negation: std::cout << "-"; break;
                case UnaryOpType::Complement: std::cout << "~"; break;
                case UnaryOpType::LogicalNegation: std::cout << "!"; break;
            }
            op->expr->Accept(this);
        }

        void VisitBinaryOp(BinaryOp* op) override
        {
            std::cout << "(";
            op->lvalue->Accept(this); 
            switch (op->opType)
            {
                case BinaryOpType::Addition: std::cout << "+"; break;
                case BinaryOpType::Subtraction: std::cout << "-"; break;
                case BinaryOpType::Multiplication: std::cout << "*"; break;
                case BinaryOpType::Division: std::cout << "/"; break;
                case BinaryOpType::LogicalOr: std::cout << "||"; break;
                case BinaryOpType::LogicalAnd: std::cout << "&&"; break;
                case BinaryOpType::Equal: std::cout << "=="; break;
                case BinaryOpType::NotEqual: std::cout << "!="; break;
                case BinaryOpType::LessThan: std::cout << "<"; break;
                case BinaryOpType::LessThanOrEqual: std::cout << "<="; break;
                case BinaryOpType::GreaterThan: std::cout << ">"; break;
                case BinaryOpType::GreaterThanOrEqual: std::cout << ">="; break;
                case BinaryOpType::Remainder: std::cout << "%"; break; 
                case BinaryOpType::BitwiseOr: std::cout << "|"; break;
                case BinaryOpType::BitwiseAnd: std::cout << "&"; break;
                case BinaryOpType::BitwiseXOR: std::cout << "^"; break;
                case BinaryOpType::BitwiseLeftShift: std::cout << "<<"; break;
                case BinaryOpType::BitwiseRightShift: std::cout << ">>"; break; 
            }
            op->rvalue->Accept(this);
            std::cout << ")";
        }

        void VisitVariableRef(VariableRef* ref) override
        {
            std::cout << ref->name; 
        }

        void VisitAssignment(Assignment* assignment) override
        {
            std::cout << assignment->lvalue << " = ";
            assignment->rvalue->Accept(this);
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

static void pretty_print_ast(AbstractSyntax* ast)
{
    if (ast != nullptr)
    {
        ASTPrinter printer{}; 
        ast->Accept(&printer); 
    } else 
    {
        std::cout << "The abstract syntax tree is null!" << std::endl; 
    }
}