#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "parser/parser.hpp"

class GenVisitor : public ASTVisitor
{
    public:
        GenVisitor(std::string filepath) 
        {
            m_Output = std::ofstream{ filepath }; 
        }

        ~GenVisitor()
        {
            m_Output.close(); 
        }

        void visitProgram(Program* program) override
        {
            visitFunction(program->function); 
        }

        void visitFunction(Function* function) override
        {
            m_Output << ".globl " << function->name << std::endl;
            m_Output << function->name << ":" << std::endl;
            // write function body
            m_IndentLevel++;
            function->statement->accept(this); 
            m_IndentLevel--;
        }

        void visitReturn(Return* ret) override
        {
            ret->expr->accept(this);
            m_Output << getSpaces() << "ret" << std::endl;
        }

        void visitIntExpr(IntExpr* expr) override
        {
            m_Output << getSpaces() << "mov " << "$" << expr->value << ", " << "%eax" << std::endl;
        }

    private: 
        std::ofstream m_Output; 
        size_t m_IndentLevel = 0; 

        std::string getSpaces()
        {
            return std::string(m_IndentLevel * 4, ' '); 
        }
};

struct GenInfo
{
    std::string name; 
};

static void genCode(AbstractSyntax* ast, GenInfo info)
{
    if (ast != nullptr)
    {
        GenVisitor gv{ info.name + ".s" };
        ast->accept(&gv);
        auto gccCmd = "gcc " + info.name + ".s -o " + info.name;
        system(gccCmd.c_str());
        auto rmCmd = "rm " + info.name + ".s"; 
        system(rmCmd.c_str());
    } else std::cout << "Could not generate code due to NULL AST!" << std::endl; 
}