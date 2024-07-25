#pragma once

#include <assert.h>

#include "gen/asm/code_gen.hpp"

// Generates x86-64 AT&T-flavor assembly
class ATTCodeGenerator : public ASMCodeGenerator
{
    public:
        void EmitFun(const std::string& name) override
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << ".globl " << name << std::endl;
            outputstream() << m_IndentStr << name << ":" << std::endl;
        }

    protected:
        std::string EvaluateArg(const RegisterArg& arg) override
        {
            return "%" + RegisterUtility::to_string(arg._register); 
        }

        std::string EvaluateArg(const PointerArg& arg) override
        {
            return std::to_string(arg.offset) + "(%" + RegisterUtility::to_string(arg._register) + ")";
        }

        std::string EvaluateArg(const ImmediateArg& arg) override
        {
            return "$" + std::to_string(arg.value);
        }

        std::string EvaluateArg(const LabelArg& arg) override
        {
            return "_" + arg.label; 
        }
};