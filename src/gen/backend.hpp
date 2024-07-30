#pragma once

#include <assert.h>
#include <memory>

#include "gen/asm.hpp"
#include "gen/code_gen.hpp"
#include "gen/def.hpp"
#include "ir/tac.hpp"

class CompilerBackend
{
    public:
        CompilerBackend()
        {
            #ifdef __linux__
            m_CodeGenerator = CreateCodeGenerator();
            auto& ref = *dynamic_cast<ASMCodeGenerator*>(m_CodeGenerator.get()); 
            m_ASMGenerator = CreateASMGenerator(ref);  
            m_Assembler = std::make_unique<GCCAssembler>(); 
            #else
            assert("unsupported platform");
            #endif
        }

        bool GenerateCode(CompilerFlags flags, AbstractSyntax::Ref ast)
        {
            if (ast == nullptr) return false; 
            // TAC generation
            TACGenerator generator{};
            try 
            {
                generator.GenerateStatements(ast); 
            } catch (const std::exception& exc)
            {
                std::cerr << exc.what() << std::endl; 
                return false; 
            }
            generator.LogStatements();
            // output
            auto outputpath = flags.outputpath + ".s"; 
            auto os = std::make_shared<std::ofstream>(outputpath); 
            m_CodeGenerator->SetOutputStream(os);
            try 
            {
                m_ASMGenerator->GenerateAssembly(generator); 
            } catch (const std::exception& exc)
            {
                std::cerr << exc.what() << std::endl; 
                // clean up file I/O
                std::remove(outputpath.c_str());
                os->close(); 
                return false; 
            }
            os->close(); 
            m_Assembler->AssembleProgram(flags.outputpath + ".s", flags.outputpath);
            return true;
        }

    private:
        std::shared_ptr<CodeGenerator> m_CodeGenerator;
        std::shared_ptr<ASMGenerator> m_ASMGenerator;

        std::unique_ptr<Assembler> m_Assembler;  

};