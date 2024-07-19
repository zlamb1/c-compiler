#pragma once

#include <assert.h>
#include <memory>

#include "gen/asm.hpp"
#include "gen/code_gen.hpp"
#include "gen/def.hpp"
#include "parser/syntax/visitor.hpp"

class CompilerBackend
{
    public:
        CompilerBackend()
        {
            #ifdef __linux__
            m_CodeGenerator = CreateCodeGenerator();
            auto& ref = *dynamic_cast<ASMCodeGenerator*>(m_CodeGenerator.get()); 
            m_Visitor = CreateASTVisitor(ref);  
            m_Assembler = std::make_unique<GCCAssembler>(); 
            #else
            assert("unsupported platform");
            #endif
        }

        void GenerateCode(CompilerFlags flags, AbstractSyntax* ast)
        {
            if (ast == nullptr) return; 
            auto os = std::make_shared<std::ofstream>(flags.outputpath + ".s"); 
            m_CodeGenerator->SetOutputStream(os);
            ast->Accept(m_Visitor.get()); 
            os->close(); 
            m_Assembler->AssembleProgram(flags.outputpath + ".s", flags.outputpath);
        }

    private:
        std::shared_ptr<CodeGenerator> m_CodeGenerator;
        std::shared_ptr<ASTVisitor> m_Visitor;

        std::unique_ptr<Assembler> m_Assembler;  

};