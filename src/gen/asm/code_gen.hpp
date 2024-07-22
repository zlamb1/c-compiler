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

        void EmitOp(const std::string& op, const AssemblyArg& arg1)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg1) << std::endl; 
        }

        void EmitOp(const std::string& op, const AssemblyArg&& arg1) { EmitOp(op, arg1); }

        void EmitOp(const std::string& op, const AssemblyArg& arg1, const AssemblyArg& arg2)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << op << " " << EvaluateArg(arg1) << ", " << EvaluateArg(arg2) << std::endl;
        } 

        void EmitOp(const std::string& op, const AssemblyArg&& arg1, const AssemblyArg&& arg2) { EmitOp(op, arg1, arg2); }

        void EmitLabel(const LabelArg& arg)
        {
            assert(m_OutputStream);
            outputstream() << EvaluateArg(arg) << ":" << std::endl;
        }

        void EmitLabel(const LabelArg&& arg) { EmitLabel(arg); }

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
        
        std::string EvaluateArg(const AssemblyArg& arg)
        {
            switch (arg.type())
            {
                case ArgType::Register:  return EvaluateArg(dynamic_cast<const RegisterArg&&>(arg));
                case ArgType::Offset:    return EvaluateArg(dynamic_cast<const OffsetArg&&>(arg));
                case ArgType::Immediate: return EvaluateArg(dynamic_cast<const ImmediateArg&&>(arg));
                case ArgType::Label:     return EvaluateArg(dynamic_cast<const LabelArg&&>(arg));
            }
            return std::string(); 
        }

        std::string EvaluateArg(const AssemblyArg&& arg) { return EvaluateArg(arg); }

        virtual std::string EvaluateArg(const RegisterArg& arg)  = 0;
        virtual std::string EvaluateArg(const OffsetArg& arg)    = 0; 
        virtual std::string EvaluateArg(const ImmediateArg& arg) = 0;
        virtual std::string EvaluateArg(const LabelArg& arg)     = 0; 

        std::string EvaluateArg(const RegisterArg&& arg)  { return EvaluateArg(arg); }
        std::string EvaluateArg(const OffsetArg&& arg)    { return EvaluateArg(arg); }
        std::string EvaluateArg(const ImmediateArg&& arg) { return EvaluateArg(arg); }
        std::string EvaluateArg(const LabelArg&& arg)     { return EvaluateArg(arg); }
};