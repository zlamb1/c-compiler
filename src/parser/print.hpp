#pragma once

#include <string>

#include "syntax/as.hpp"
#include "syntax/program.hpp"
#include "syntax/function.hpp"
#include "syntax/expr.hpp"

class ASTPrinter : public ASTVisitor
{
    public:
        void visitProgram(Program* program) override
        {
            std::cout << "Program(" << std::endl;
            setIndent(m_Indent + 1); 
            visitFunction(program->function);
            setIndent(m_Indent - 1);
            std::cout << ")" << std::endl;
        }

        void visitFunction(Function* function) override
        {
            std::cout << m_Spaces << function->name << "(" << std::endl;
            setIndent(m_Indent + 1); 
            function->statement->accept(this); 
            setIndent(m_Indent - 1); 
            std::cout << m_Spaces << ")" << std::endl;
        }

        void visitReturn(Return* ret) override
        {
            std::cout << m_Spaces << "Return(" << std::endl;
            setIndent(m_Indent + 1);  
            std::cout << m_Spaces; 
            ret->expr->accept(this); 
            std::cout << "\n";
            setIndent(m_Indent - 1); 
            std::cout << m_Spaces << ")" << std::endl;
        }

        void visitIntExpr(IntExpr* expr) override
        {
            std::cout << "IntExpr(" << expr->value << ")";
        }

        void visitUnaryOp(UnaryOp* op) override
        {
            switch (op->type)
            {
                case UnaryType::Negation: std::cout << "-"; break;
                case UnaryType::BWComplement: std::cout << "~"; break;
                case UnaryType::LogicalNegation: std::cout << "!"; break;
            }
            op->expr->accept(this);
        }
    
    private:
        size_t m_Indent = 0; 
        size_t m_IndentSize = 2; 

        std::string m_Spaces; 
        void setIndent(size_t indent)
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
        ast->accept(&printer); 
    } else 
    {
        std::cout << "The AST is NULL!" << std::endl; 
    }
}