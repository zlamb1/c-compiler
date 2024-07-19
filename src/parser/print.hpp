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
            function->statement->Accept(this); 
            SetIndent(m_Indent - 1); 
            std::cout << m_Spaces << ")" << std::endl;
        }

        void VisitReturn(Return* ret) override
        {
            std::cout << m_Spaces << "Return(" << std::endl;
            SetIndent(m_Indent + 1);  
            std::cout << m_Spaces; 
            ret->expr->Accept(this); 
            std::cout << "\n";
            SetIndent(m_Indent - 1); 
            std::cout << m_Spaces << ")" << std::endl;
        }

        void VisitIntExpr(IntExpr* expr) override
        {
            std::cout << "IntExpr(" << expr->value << ")";
        }

        void VisitUnaryOp(UnaryOp* op) override
        {
            switch (op->type)
            {
                case UnaryType::Negation: std::cout << "-"; break;
                case UnaryType::BWComplement: std::cout << "~"; break;
                case UnaryType::LogicalNegation: std::cout << "!"; break;
            }
            op->expr->Accept(this);
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
        std::cout << "The AST is NULL!" << std::endl; 
    }
}