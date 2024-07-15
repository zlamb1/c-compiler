#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "compiler.hpp"

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

        void visitUnaryOp(UnaryOp* op) override
        {
            auto spaces = getSpaces(); 
            op->expr->accept(this); 
            std::string assembly; 
            switch (op->type)
            {
                case UnaryType::Negation:
                    assembly = "neg %eax";
                    break;
                case UnaryType::BWComplement:
                    assembly = "not %eax";
                    break;
                case UnaryType::LogicalNegation:
                    assembly = "cmpl $0, %eax\n";
                    assembly += spaces + "movl $0, %eax\n";
                    assembly += spaces + "sete %al";
                    break;
            }
            m_Output << spaces << assembly << std::endl; 
        }

    private: 
        std::ofstream m_Output; 
        size_t m_IndentLevel = 0; 

        std::string getSpaces()
        {
            return std::string(m_IndentLevel * 4, ' '); 
        }
};

static void genCode(AbstractSyntax* ast, CompilerFlags& flags)
{
    if (ast != nullptr)
    {
        GenVisitor gv{ flags.outputname + ".s" };
        ast->accept(&gv);
        auto gccCmd = "gcc " + flags.outputname + ".s -o " + flags.outputname;
        system(gccCmd.c_str());
        if (!flags.output_asm)
        {
            auto rmCmd = "rm " + flags.outputname + ".s"; 
            system(rmCmd.c_str());
        }
    } else std::cout << "Could not generate code due to NULL abstract syntax tree!" << std::endl; 
}