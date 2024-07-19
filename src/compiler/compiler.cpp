#include "compiler.hpp"

Compiler::Compiler(int argc, char* argv[])
{
    ScanFlags(argc, argv); 
    m_Parser = std::make_unique<RDParser>(std::make_unique<Lexer>());
    m_CompilerBackend = std::make_unique<CompilerBackend>(); 
}

Compiler::Compiler(const std::string& filepath, int argc, char* argv[])
{
    m_Flags.filepath = filepath; 
    ScanFlags(argc, argv);
    m_Parser = std::make_unique<RDParser>(std::make_unique<Lexer>());
    m_CompilerBackend = std::make_unique<CompilerBackend>(); 
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
    
    auto ast = m_Parser->ParseFile(m_Flags.filepath); 
    m_Parser->PrintTree(ast); 

    if (ast == nullptr)
    {
        LogError("Failed to generate abstract syntax tree."); 
        return;
    }

    m_CompilerBackend->GenerateCode(m_Flags, ast); 
}

void Compiler::ScanFlags(int argc, char* argv[])
{
    if (argc > 1)
        m_Flags.filepath = argv[1]; 

    auto& filepath = m_Flags.filepath; 
    if (!filepath.empty() && filepath.substr(filepath.length() - 2) == ".c")
        m_Flags.outputpath = filepath.substr(0, filepath.length() - 2); 

    for (int i = 0; i < argc; i++)
    {
        const char* arg = argv[i]; 
        if (memcmp(arg, "-s", 2) == 0 || memcmp(arg, "-S", 2) == 0)
            m_Flags.output_asm = true;
    }
}

void Compiler::LogWarn(const char* msg)
{
    std::cout << "[CWarning] " << msg << std::endl; 
}

void Compiler::LogError(const char* msg)
{
    std::cout << "[CError] " << msg << std::endl; 
}