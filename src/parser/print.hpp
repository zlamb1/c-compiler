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
            m_Spaces += m_Indent; 
            visitFunction(program->function);
            m_Spaces -= m_Indent;
            std::cout << ")" << std::endl;
        }

        void visitFunction(Function* function) override
        {
            std::string spaces(m_Spaces, ' '); 
            std::cout << spaces << function->name << "(" << std::endl;
            m_Spaces += m_Indent; 
            function->statement->accept(this); 
            m_Spaces -= m_Indent; 
            std::cout << spaces << ")" << std::endl;
        }

        void visitReturn(Return* ret) override
        {
            std::string spaces(m_Spaces, ' '); 
            std::cout << spaces << "Return(" << std::endl;
            m_Spaces += m_Indent; 
            ret->expr->accept(this); 
            m_Spaces -= m_Indent; 
            std::cout << spaces << ")" << std::endl;
        }

        void visitIntExpr(IntExpr* expr) override
        {
            std::string spaces(m_Spaces, ' '); 
            std::cout << spaces << "Expr(" << expr->value << ")" << std::endl;
        }
    
    private:
        size_t m_Spaces = 0; 
        size_t m_Indent = 2; 
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