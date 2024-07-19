#pragma once

#include "generator.hpp"
#include "asm/att/code_gen.hpp"

// construct a default code generator for arch
static std::shared_ptr<CodeGenerator> CreateCodeGenerator()
{
    #ifdef __linux__
    return std::make_shared<ATTCodeGenerator>(); 
    #endif
    return nullptr; 
}

static std::shared_ptr<ASTVisitor> CreateASTVisitor(ASMCodeGenerator& codeGenerator)
{
    #ifdef __linux__
    return std::make_shared<ASMVisitor>(codeGenerator); 
    #endif
    return nullptr; 
}