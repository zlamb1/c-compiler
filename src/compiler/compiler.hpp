#pragma once

#include <assert.h>
#include <cstring>
#include <memory>

#include "flags.hpp"
#include "gen/backend.hpp"
#include "gen/code_gen.hpp"
#include "gen/opt/constant_folding.hpp"
#include "gen/def.hpp"
#include "lexer/lexer.hpp"
#include "parser/rd_parser.hpp"

class Compiler
{
    public:
        Compiler(int argc, char* argv[]);
        Compiler(const std::string& filepath, int argc, char* argv[]); 

        CompilerFlags& GetFlags(); 

        void CompileProgram();

    private:
        CompilerFlags m_Flags; 
        std::unique_ptr<Parser> m_Parser; 
        std::unique_ptr<CompilerBackend> m_CompilerBackend; 
        
        void ScanFlags(int argc, char* argv[]); 
        void LogWarn(const char* msg); 
        void LogError(const char* msg); 
};