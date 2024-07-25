#pragma once

#include "asm/at&t/code_gen.hpp"
#include "asm/generator.hpp"

// construct a default code generator for arch
static std::shared_ptr<CodeGenerator> CreateCodeGenerator()
{
    #ifdef __linux__
    return std::make_shared<ATTCodeGenerator>(); 
    #endif
    return nullptr; 
}

static std::shared_ptr<ASMGenerator> CreateASMGenerator(ASMCodeGenerator& codeGenerator)
{
    #ifdef __linux__
    return std::make_shared<ASMGenerator>(codeGenerator); 
    #endif
    return nullptr; 
}