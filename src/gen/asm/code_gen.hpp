#pragma once

#include <memory>
#include <ostream>
#include <string>

#include "arg.hpp"
#include "gen/code_gen.hpp"
#include "instruction.hpp"

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

        std::optional<OperandSize> InferSize(OpInstruction op, const AssemblyArg& dst)
        {
            switch (dst.type())
            {
                case ArgType::Register:
                    return std::make_optional<OperandSize>(RegisterUtility::register_size(dynamic_cast<const RegisterArg&>(dst)._register));
            }                
            return std::nullopt;
        }

        std::optional<OperandSize> InferSize(OpInstruction op, const AssemblyArg& src, const AssemblyArg& dst)
        {
            auto src_size = InferSize(op, src); 
            if (src_size) return src_size;
            else return InferSize(op, dst); 
        }

        std::string FormatInstruction(OpInstruction op, OperandSize s)
        {
            switch (s)
            {
                case OperandSize::BYTE:  
                     return InstructionUtility::to_string<OperandSize::BYTE>(op);
                case OperandSize::WORD:  
                    return InstructionUtility::to_string<OperandSize::WORD>(op);
                case OperandSize::DWORD: 
                    return InstructionUtility::to_string<OperandSize::DWORD>(op);
                case OperandSize::QWORD: 
                    return InstructionUtility::to_string<OperandSize::QWORD>(op);
                default: return std::string(); 
            }
        }

        void EmitOp(OpInstruction op)
        {
            assert(m_OutputStream);
            outputstream() << m_IndentStr << InstructionUtility::to_string(op) << "\n";
        }

        template<OperandSize S>
        void EmitOp(OpInstruction op, const AssemblyArg& dst)
        {
            assert(m_OutputStream); 
            outputstream() << m_IndentStr << InstructionUtility::to_string<S>(op) << " " << EvaluateArg(dst) << "\n";
        }

        template<OperandSize S>
        void EmitOp(OpInstruction op, AssemblyArg::Ref dst) { EmitOp<S>(op, *dst.get()); }

        void EmitOp(OpInstruction op, const AssemblyArg& dst)
        {
            assert(m_OutputStream);
            auto inferredSize = InferSize(op, dst);
            std::string opString = InstructionUtility::to_string(op);
            if (InstructionUtility::needs_operand_size(op))
            {
                if (inferredSize) opString = FormatInstruction(op, inferredSize.value()); 
                else std::cout << "warning: Could not infer size for '" + opString +  "'\n";
            }
            outputstream() << m_IndentStr << opString << " " << EvaluateArg(dst) << "\n"; 
        }

        void EmitOp(OpInstruction op, const AssemblyArg&& dst) { EmitOp(op, dst); }
        void EmitOp(OpInstruction op, AssemblyArg::Ref dst) { EmitOp(op, *dst.get()); }

        template<OperandSize S>
        void EmitOp(OpInstruction op, const AssemblyArg& src, const AssemblyArg& dst)
        {
            assert(m_OutputStream); 
            outputstream() << m_IndentStr << InstructionUtility::to_string<S>(op) << " " << EvaluateArg(src) << ", " << EvaluateArg(dst) << "\n";
        }

        void EmitOp(OpInstruction op, const AssemblyArg& src, const AssemblyArg& dst)
        {
            assert(m_OutputStream);
            auto inferredSize = InferSize(op, src, dst);
            std::string opString = InstructionUtility::to_string(op);
            if (InstructionUtility::needs_operand_size(op))
            {
                if (inferredSize) opString = FormatInstruction(op, inferredSize.value()); 
                else std::cout << "warning: Could not infer size for '" + opString +  "'\n";
            }
            outputstream() << m_IndentStr << opString << " " << EvaluateArg(src) << ", " << EvaluateArg(dst) << "\n"; 
        } 

        void EmitOp(OpInstruction op, const AssemblyArg& src, AssemblyArg::Ref dst) { EmitOp(op, src, *dst.get()); }
        void EmitOp(OpInstruction op, AssemblyArg::Ref src, const AssemblyArg& dst) { EmitOp(op, *src.get(), dst); }
        void EmitOp(OpInstruction op, const AssemblyArg&& src, const AssemblyArg&& dst) { EmitOp(op, src, dst); }
        void EmitOp(OpInstruction op, AssemblyArg::Ref src, const AssemblyArg&& dst) { EmitOp(op, *src.get(), dst); }
        void EmitOp(OpInstruction op, const AssemblyArg&& src, AssemblyArg::Ref dst) { EmitOp(op, src, *dst.get()); }
        void EmitOp(OpInstruction op, AssemblyArg::Ref src, AssemblyArg::Ref dst) { EmitOp(op, *src.get(), *dst.get()); }

        void EmitLabel(const LabelArg& label)
        {
            assert(m_OutputStream);
            outputstream() << EvaluateArg(label) << ":" << std::endl;
        }

        void EmitLabel(const LabelArg&& label) { EmitLabel(label); }

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
                case ArgType::Register:   return EvaluateArg(dynamic_cast<const RegisterArg&&>(arg));
                case ArgType::Pointer:    return EvaluateArg(dynamic_cast<const PointerArg&&>(arg));
                case ArgType::Immediate:  return EvaluateArg(dynamic_cast<const ImmediateArg&&>(arg));
                case ArgType::Label:      return EvaluateArg(dynamic_cast<const LabelArg&&>(arg));
            }
            return std::string(); 
        }

        std::string EvaluateArg(const AssemblyArg&& arg) { return EvaluateArg(arg); }

        virtual std::string EvaluateArg(const RegisterArg& arg)  = 0;
        virtual std::string EvaluateArg(const PointerArg& arg)   = 0; 
        virtual std::string EvaluateArg(const ImmediateArg& arg) = 0;
        virtual std::string EvaluateArg(const LabelArg& arg)     = 0; 

        std::string EvaluateArg(const RegisterArg&& arg)  { return EvaluateArg(arg); }
        std::string EvaluateArg(const PointerArg&& arg)   { return EvaluateArg(arg); }
        std::string EvaluateArg(const ImmediateArg&& arg) { return EvaluateArg(arg); }
        std::string EvaluateArg(const LabelArg&& arg)     { return EvaluateArg(arg); }
};