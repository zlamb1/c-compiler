#include "compiler.hpp"

#include <cstring>

#include "gen/code_gen.hpp"

Compiler::Compiler(int argc, char* argv[], Parser& parser) :
    m_Parser(parser)
{
    ScanFlags(argc, argv); 
}

Compiler::Compiler(std::string filepath, int argc, char* argv[], Parser& parser) :
    m_Parser(parser)
{
    m_Flags.filepath = filepath; 
    ScanFlags(argc, argv); 
}

CompilerFlags& Compiler::GetFlags()
{
    return m_Flags; 
}

void Compiler::CompileProgram()
{
    if (m_Flags.filepath.empty())
    {
        LogWarn("Cannot compile program with empty file path!"); 
        return;
    }
    
    auto ast = m_Parser.ParseFile(m_Flags.filepath); 
    m_Parser.PrintTree(ast); 

    if (ast == nullptr)
    {
        LogError("Failed to create abstract syntax tree."); 
        return;
    }

    genCode(ast, m_Flags); 
}

void Compiler::ScanFlags(int argc, char* argv[])
{
    if (argc > 1)
        m_Flags.filepath = argv[1]; 

    auto& filepath = m_Flags.filepath; 
    if (!filepath.empty())
        if (filepath.substr(filepath.length() - 2) == ".c")
            m_Flags.outputname = filepath.substr(0, filepath.length() - 2); 

    for (int i = 0; i < argc; i++)
    {
        const char* arg = argv[i]; 
        if (memcmp(arg, "-s", 2) == 0 || memcmp(arg, "-S", 2) == 0)
            m_Flags.output_asm = true;
    }
}

void Compiler::LogWarn(const char* msg)
{
    std::cout << "[Compiler Warning] >> " << msg << std::endl; 
}

void Compiler::LogError(const char* msg)
{
    std::cout << "[Compiler Error] >> " << msg << std::endl; 
}