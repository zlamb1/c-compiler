#pragma once

#include <memory>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

struct CompilerFlags
{
    std::string filepath;
    std::string outputname = "a.out"; 
    bool output_asm = false; 
};

class Compiler
{
    public:
        Compiler(int argc, char* argv[], Parser& parser); 
        Compiler(std::string filepath, int argc, char* argv[], Parser& parser);

        CompilerFlags& GetFlags(); 

        void CompileProgram();

    private:
        CompilerFlags m_Flags; 
        Parser& m_Parser; 
        
        void ScanFlags(int argc, char* argv[]); 
        void LogWarn(const char* msg); 
        void LogError(const char* msg); 
};