#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "arg.hpp"
#include "code_gen.hpp"
#include "instruction.hpp"
#include "register.hpp"

#include "gen/context.hpp"
#include "gen/exc.hpp"
#include "parser/parser.hpp"

class ASMGenerator
{
    public:
        ASMGenerator(ASMCodeGenerator& codeGenerator) : m_CodeGenerator(codeGenerator)
        {
            m_DstStack.push(RegisterArg(Register::EAX));
        }

        void GenerateSyntax(AbstractSyntax::Ref syntax);
        void GenerateProgram(Program::Ref program);
        void GenerateFunction(Function::Ref function);
        void GenerateStatementExpression(StatementExpression::Ref statementExpression);
        void GenerateAssignmentOp(AssignmentOp::Ref op);
        void GenerateDeclaration(Declaration::Ref decl);
        void GenerateCompoundBlock(CompoundBlock::Ref compoundBlock);
        void GenerateIfStatement(IfStatement::Ref ifStatement);
        void GenerateReturn(Return::Ref ret);
        void GenerateIntConstant(IntConstant::Ref expr);
        void GenerateUnaryOp(UnaryOp::Ref op);
        void GenerateBinaryOp(BinaryOp::Ref op);
        void GenerateVariableRef(VariableRef::Ref ref);
        void GenerateAssignment(Assignment::Ref assignment);

    private: 
        ASMCodeGenerator& m_CodeGenerator; 
        std::stack<BlockContext::Ref> m_ContextStack;
        std::stack<RegisterArg> m_DstStack; 

        const RegisterArg& ActiveRegister()
        {
            return m_DstStack.top(); 
        }

        void ZeroRegister(Register _register)
        {
            auto arg = RegisterArg(_register);
            m_CodeGenerator.EmitOp(OpInstruction::XOR, arg, arg);
        }
        
        AssemblyArg::Ref LoadRegister(Expression::Ref expr, Register dst)
        {
            PushRegister(dst); 
            GenerateSyntax(expr);
            PopRegister();
            return CreateRef<RegisterArg>(dst); 
        }

        void PushRegister(Register _register)
        {
            m_DstStack.push(RegisterArg(_register)); 
        }

        void PopRegister()
        {
            m_DstStack.pop();
        }

        size_t AccumVarSize(AbstractSyntax::Ref syntax)
        {
            switch (syntax->type())
            {
                case SyntaxType::Declaration:
                {
                    auto decl = AbstractSyntax::RefCast<Declaration>(syntax);
                    return decl->type_size;
                }
                case SyntaxType::CompoundBlock:
                {
                    auto block = AbstractSyntax::RefCast<CompoundBlock>(syntax);
                    size_t accumulator = 0;
                    for (auto statement : block->statements)
                        accumulator += AccumVarSize(statement); 
                    return accumulator;
                }
                case SyntaxType::IfStatement:
                {
                    auto if_statement = AbstractSyntax::RefCast<IfStatement>(syntax);
                    size_t accumulator = 0;
                    accumulator += AccumVarSize(if_statement->if_conditional.statement);
                    for (auto else_if : if_statement->else_ifs)
                        accumulator += AccumVarSize(else_if.statement);
                    if (if_statement->else_statement != nullptr)
                        accumulator += AccumVarSize(if_statement->else_statement); 
                    return accumulator; 
                }
            }
            return 0;
        }
};