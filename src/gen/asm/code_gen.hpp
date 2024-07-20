#pragma once

#include <memory>
#include <ostream>
#include <string>

#include "gen/code_gen.hpp"

class ASMCodeGenerator : public CodeGenerator
{
    public:
        void IncreaseIndentation()
        {
            m_Indents++;
            m_IndentStr = std::string(m_IndentSize * m_Indents, ' ');
        }

        void DecreaseIndentation()
        {
            m_Indents--;
            m_IndentStr = std::string(m_IndentSize * m_Indents, ' ');
        }

        void EmitOp(const std::string& op)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << std::endl;
        }

        void EmitOp(const std::string& op, const RegisterArg& arg)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg) << std::endl;
        }

        void EmitOp(const std::string& op, const RegisterArg& arg1, const RegisterArg& arg2)
        {
            assert(m_OutputStream); 
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg1) << ", " << EvaluateArg(arg2) << std::endl; 
        }

        void EmitOp(const std::string& op, const ImmediateArg& arg1, const RegisterArg& arg2)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg1) << ", " << EvaluateArg(arg2) << std::endl;
        }

        void EmitOp(const std::string& op, const LabelArg& arg1)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg1) << std::endl;
        }

        void EmitLabel(const LabelArg& arg)
        {
            assert(m_OutputStream);
            outputstream() << EvaluateArg(arg) << ":" << std::endl;
        }

        LabelArg GenerateLabel()
        {
            return LabelArg("L" + std::to_string(m_LabelCount++));
        }

        virtual void EmitFun(const std::string& name) = 0;

    protected:
        size_t m_IndentSize = 4; 
        size_t m_Indents = 0; 

        size_t m_LabelCount = 0; 

        std::string m_IndentStr; 

        virtual std::string EvaluateArg(const RegisterArg& arg) = 0;
        virtual std::string EvaluateArg(const ImmediateArg& arg) = 0;
        virtual std::string EvaluateArg(const LabelArg& arg) = 0; 
};