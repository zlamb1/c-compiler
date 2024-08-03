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
                case SyntaxType::DoWhile:
                    PrintDoWhileStatement(AbstractSyntax::RefCast<DoWhileStatement>(syntax));
                    break;
                case SyntaxType::While:
                    PrintWhileStatement(AbstractSyntax::RefCast<WhileStatement>(syntax));
                    break;
                case SyntaxType::ForDecl:
                    PrintForDeclStatement(AbstractSyntax::RefCast<ForDeclStatement>(syntax));
                    break;
                case SyntaxType::For:
                    PrintForStatement(AbstractSyntax::RefCast<ForStatement>(syntax));
                    break;
                case SyntaxType::Break:
                    PrintBreakStatement(AbstractSyntax::RefCast<BreakStatement>(syntax));
                    break;
                case SyntaxType::Continue:
                    PrintContinueStatement(AbstractSyntax::RefCast<ContinueStatement>(syntax));
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

        void PrintDoWhileStatement(DoWhileStatement::Ref do_while_statement)
        {
            m_OutputStream << m_Spaces;
            m_OutputStream << "do\n";
            if (do_while_statement->body->type() != SyntaxType::CompoundBlock)
                m_OutputStream << std::string(m_IndentSize, ' ');
            PrintSyntax(do_while_statement->body);
            m_OutputStream << m_Spaces << "while (";
            PrintSyntax(do_while_statement->condition);
            m_OutputStream << ");\n";
        }

        void PrintWhileStatement(WhileStatement::Ref while_statement)
        {
            m_OutputStream << m_Spaces;
            m_OutputStream << "while (";
            PrintSyntax(while_statement->condition);
            m_OutputStream << ")\n";
            if (while_statement->body->type() != SyntaxType::CompoundBlock)
                m_OutputStream << std::string(m_IndentSize, ' ');
            PrintSyntax(while_statement->body);
        }

        void PrintForDeclStatement(ForDeclStatement::Ref for_decl_statement)
        {
            m_OutputStream << m_Spaces << "for (";
            PrintDeclaration(for_decl_statement->declaration, false); 
            m_OutputStream << " "; 
            PrintSyntax(for_decl_statement->condition);
            m_OutputStream << "; ";
            PrintSyntax(for_decl_statement->post_expression);
            m_OutputStream << ")\n";
            if (for_decl_statement->body->type() != SyntaxType::CompoundBlock)
                m_OutputStream << std::string(m_IndentSize, ' ');
            PrintSyntax(for_decl_statement->body);
        }

        void PrintForStatement(ForStatement::Ref for_statement)
        {
            m_OutputStream << m_Spaces << "for (";
            PrintSyntax(for_statement->expression); 
            m_OutputStream << ";"; 
            if (for_statement->condition->type() != SyntaxType::Null) 
                m_OutputStream << " ";
            PrintSyntax(for_statement->condition);
            m_OutputStream << ";";
            if (for_statement->post_expression->type() != SyntaxType::Null) 
                m_OutputStream << " ";
            PrintSyntax(for_statement->post_expression);
            m_OutputStream << ")\n";
            if (for_statement->body->type() != SyntaxType::CompoundBlock)
                m_OutputStream << std::string(m_IndentSize, ' ');
            PrintSyntax(for_statement->body);
        }

        void PrintBreakStatement(BreakStatement::Ref break_statement)
        {
            m_OutputStream << m_Spaces << "break;\n";
        }

        void PrintContinueStatement(ContinueStatement::Ref continue_statement)
        {
            m_OutputStream << m_Spaces << "continue;\n";
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

        void PrintDeclaration(Declaration::Ref decl, bool use_space = true)
        {
            size_t num_vars = decl->variables.size(); 
            if (use_space) m_OutputStream << m_Spaces << "int "; 
            else m_OutputStream << "int ";
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
            m_OutputStream << ";"; 
            if (use_space) m_OutputStream << "\n";
        }

        void PrintCompoundBlock(CompoundBlock::Ref compoundBlock)
        {
            m_OutputStream << m_Spaces << "{\n";
            SetIndent(m_Indent + 1); 
            if (compoundBlock->statements.empty()) 
                m_OutputStream << "\n";
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

        void PrintIfStatement(IfStatement::Ref if_statement)
        {
            m_OutputStream << m_Spaces << "IF (";
            PrintSyntax(if_statement->if_conditional.condition); 
            m_OutputStream << ")\n";
            if (if_statement->if_conditional.statement->type() != SyntaxType::CompoundBlock)
                m_OutputStream << std::string(m_IndentSize, ' ');
            PrintBlockItem(if_statement->if_conditional.statement);
            for (auto else_if : if_statement->else_ifs)
            {
                m_OutputStream << m_Spaces << "ELSE IF (";
                PrintSyntax(else_if.condition); 
                m_OutputStream << ")\n"; 
                if (else_if.statement->type() != SyntaxType::CompoundBlock)
                    m_OutputStream << std::string(m_IndentSize, ' ');
                PrintBlockItem(else_if.statement); 
            }
            if (if_statement->else_statement != nullptr)
            {
                m_OutputStream << m_Spaces << "ELSE\n";
                if (if_statement->else_statement->type() != SyntaxType::CompoundBlock)
                    m_OutputStream << std::string(m_IndentSize, ' ');
                PrintBlockItem(if_statement->else_statement); 
            }
        }

        void PrintReturn(Return::Ref ret)
        {
            m_OutputStream << m_Spaces << "return ";
            SetIndent(m_Indent + 1);  
            PrintSyntax(ret->expr);
            SetIndent(m_Indent - 1); 
            m_OutputStream << ";\n";
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
            PrintSyntax(op->lvalue);
            m_OutputStream << " ";
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
            m_OutputStream << " ";
            PrintSyntax(op->rvalue);
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