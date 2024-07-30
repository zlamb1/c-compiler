#pragma once

#include <exception>

#include "operand_size.hpp"

enum class OpInstruction : int
{
    MOV,
    MOVZ,
    NOT,
    NEG,
    ADD,
    SUB,
    IMUL,
    IDIV,
    CDQ,
    CMP,
    SETE,
    SETNE,
    SETL,
    SETLE,
    SETG,
    SETGE,
    JE,
    JNE,
    JMP,
    AND,
    OR,
    XOR,
    SAL,
    SAR,
    PUSH,
    POP,
    RET
};

namespace InstructionUtility
{
    static inline std::string to_string(OpInstruction op)
    {
        switch (op)
        {
            case OpInstruction::MOV:   return "mov";
            case OpInstruction::MOVZ:  return "movz"; 
            case OpInstruction::NOT:   return "not";
            case OpInstruction::NEG:   return "neg";
            case OpInstruction::ADD:   return "add";
            case OpInstruction::SUB:   return "sub";
            case OpInstruction::IMUL:  return "imul";
            case OpInstruction::IDIV:  return "idiv";
            case OpInstruction::CDQ:   return "cdq";
            case OpInstruction::CMP:   return "cmp";
            case OpInstruction::SETE:  return "sete";
            case OpInstruction::SETNE: return "setne";
            case OpInstruction::SETL:  return "setl";
            case OpInstruction::SETLE: return "setle";
            case OpInstruction::SETG:  return "setg";
            case OpInstruction::SETGE: return "setge";
            case OpInstruction::JE:    return "je";
            case OpInstruction::JNE:   return "jne";
            case OpInstruction::JMP:   return "jmp";
            case OpInstruction::AND:   return "and";
            case OpInstruction::OR:    return "or";
            case OpInstruction::XOR:   return "xor";
            case OpInstruction::SAL:   return "sal";
            case OpInstruction::SAR:   return "sar"; 
            case OpInstruction::PUSH:  return "push";
            case OpInstruction::POP:   return "pop";
            case OpInstruction::RET:   return "ret"; 
            default: 
                throw std::runtime_error("error: Unknown operation '" + std::to_string((int)op) + "'");
        }
        return std::string();
    }   

    // define permitted operand sizes for ops

    static inline bool QWORD(OpInstruction op)
    {
        switch (op)
        {
        }
        return true;
    }

    static inline bool DWORD(OpInstruction op)
    {
        switch (op)
        {
            case OpInstruction::PUSH:
            case OpInstruction::POP:  
                return false;
        }
        return true;
    }

    static inline bool WORD(OpInstruction op)
    {
        switch (op) 
        {
            case OpInstruction::PUSH:
            case OpInstruction::POP:  
                return false;
        }
        return true; 
    }

    static inline bool BYTE(OpInstruction op)
    {
        switch (op) 
        {
            case OpInstruction::PUSH:
            case OpInstruction::POP:
                return false; 
        }
        return true; 
    }

    static inline bool needs_operand_size(OpInstruction op)
    {
        switch (op)
        {
            case OpInstruction::IDIV:
            case OpInstruction::JE:
            case OpInstruction::JNE:
            case OpInstruction::JMP:
                return false;
        }
        return true;
    }

    template<OperandSize S>
    static inline std::string to_string(OpInstruction op)
    {
        switch (S)
        {
            case OperandSize::BYTE:
                if (!BYTE(op)) throw std::runtime_error("error: Invalid operand size BYTE for '" + to_string(op) + "'");
                return to_string(op) + "b";
            case OperandSize::WORD:
                if (!WORD(op)) throw std::runtime_error("error: Invalid operand size WORD for '" + to_string(op) + "'"); 
                return to_string(op) + "w"; 
            case OperandSize::DWORD:
                if (!DWORD(op)) throw std::runtime_error("error: Invalid operand size DWORD for '" + to_string(op) + "'");
                return to_string(op) + "l";
            case OperandSize::QWORD:
                if (!QWORD(op)) throw std::runtime_error("error: Invalid operand size QWORD for '" + to_string(op) + "'");
                return to_string(op) + "q";
            default:
                throw std::runtime_error("error: Unsupported operand size");
        }
        return std::string(); 
    }
};