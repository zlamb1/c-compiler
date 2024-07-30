#pragma once

#include <iostream>
#include <optional>
#include <stdexcept>

#include "operand_size.hpp"

enum class Register : int
{
    // BYTE
    AH, AL,
    BH, BL,
    CH, CL,
    DH, DL,
    SIL,
    DIL,
    BPL,
    SPL,
    R8B,  R9B,  R10B, R11B,
    R12B, R13B, R14B, R15B,
    // WORD
    AX,
    BX,
    CX,
    DX,
    SI,
    DI,
    BP,
    SP,
    R8W,  R9W,  R10W, R11W,
    R12W, R13W, R14W, R15W,
    // DWORD
    EAX,
    EBX,
    ECX,
    EDX,
    ESI,
    EDI,
    EBP,
    ESP,
    R8D,  R9D,  R10D, R11D,
    R12D, R13D, R14D, R15D,
    // QWORD
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,  R9,  R10, R11, 
    R12, R13, R14, R15,
};

namespace RegisterUtility
{
    static inline std::string to_string(Register _register)
    {
        switch (_register)
        {
            case Register::AH:   return "ah";
            case Register::AL:   return "al";
            case Register::BH:   return "bh";
            case Register::BL:   return "bl";
            case Register::CH:   return "ch";
            case Register::CL:   return "cl";
            case Register::DH:   return "dh";
            case Register::DL:   return "dl";
            case Register::SIL:  return "sil";
            case Register::DIL:  return "dil"; 
            case Register::BPL:  return "bpl";
            case Register::SPL:  return "spl";
            case Register::R8B:  return "r8b";
            case Register::R9B:  return "r9b";
            case Register::R10B: return "r10b";
            case Register::R11B: return "r11b";
            case Register::R12B: return "r12b";
            case Register::R13B: return "r13b";
            case Register::R14B: return "r14b"; 
            case Register::R15B: return "r15b";
            case Register::AX:   return "ax";
            case Register::BX:   return "bx";
            case Register::CX:   return "cx";
            case Register::DX:   return "dx";
            case Register::SI:   return "si";
            case Register::DI:   return "di";
            case Register::BP:   return "bp";
            case Register::SP:   return "sp";
            case Register::R8W:  return "r8w";
            case Register::R9W:  return "r9w";
            case Register::R10W: return "r10w";
            case Register::R11W: return "r11w";
            case Register::R12W: return "r12w";
            case Register::R13W: return "r13w";
            case Register::R14W: return "r14w";
            case Register::R15W: return "r15w"; 
            case Register::EAX:  return "eax";
            case Register::EBX:  return "ebx";
            case Register::ECX:  return "ecx";
            case Register::EDX:  return "edx";
            case Register::ESI:  return "esi";
            case Register::EDI:  return "edi";
            case Register::EBP:  return "ebp";
            case Register::ESP:  return "esp";
            case Register::R8D:  return "r8d"; 
            case Register::R9D:  return "r9d";
            case Register::R10D: return "r10d";
            case Register::R11D: return "r11d";
            case Register::R12D: return "r12d";
            case Register::R13D: return "r13d";
            case Register::R14D: return "r14d";
            case Register::R15D: return "r15d"; 
            case Register::RAX:  return "rax";
            case Register::RBX:  return "rbx";
            case Register::RCX:  return "rcx";
            case Register::RDX:  return "rdx";
            case Register::RSI:  return "rsi";
            case Register::RDI:  return "rbi";
            case Register::RBP:  return "rbp";
            case Register::RSP:  return "rsp"; 
            case Register::R8:   return "r8"; 
            case Register::R9:   return "r9"; 
            case Register::R10:  return "r10";
            case Register::R11:  return "r11";
            case Register::R12:  return "r12";
            case Register::R13:  return "r13";
            case Register::R14:  return "r14";
            case Register::R15:  return "r15";
            default: 
                throw std::runtime_error("error: Unknown register '" + std::to_string((int)_register) + "'");
        }
        return std::string("UNKNOWN_REGISTER"); 
    }

    static inline OperandSize get_register_size(Register _register)
    {
        switch (_register)
        {
            case Register::AH:   return OperandSize::BYTE;
            case Register::AL:   return OperandSize::BYTE;
            case Register::BH:   return OperandSize::BYTE;
            case Register::BL:   return OperandSize::BYTE;
            case Register::CH:   return OperandSize::BYTE;
            case Register::CL:   return OperandSize::BYTE;
            case Register::DH:   return OperandSize::BYTE;
            case Register::DL:   return OperandSize::BYTE;
            case Register::SIL:  return OperandSize::BYTE;
            case Register::DIL:  return OperandSize::BYTE; 
            case Register::BPL:  return OperandSize::BYTE;
            case Register::SPL:  return OperandSize::BYTE;
            case Register::R8B:  return OperandSize::BYTE;
            case Register::R9B:  return OperandSize::BYTE;
            case Register::R10B: return OperandSize::BYTE;
            case Register::R11B: return OperandSize::BYTE;
            case Register::R12B: return OperandSize::BYTE;
            case Register::R13B: return OperandSize::BYTE;
            case Register::R14B: return OperandSize::BYTE; 
            case Register::R15B: return OperandSize::BYTE;
            case Register::AX:   return OperandSize::WORD;
            case Register::BX:   return OperandSize::WORD;
            case Register::CX:   return OperandSize::WORD;
            case Register::DX:   return OperandSize::WORD;
            case Register::SI:   return OperandSize::WORD;
            case Register::DI:   return OperandSize::WORD;
            case Register::BP:   return OperandSize::WORD;
            case Register::SP:   return OperandSize::WORD;
            case Register::R8W:  return OperandSize::WORD;
            case Register::R9W:  return OperandSize::WORD;
            case Register::R10W: return OperandSize::WORD;
            case Register::R11W: return OperandSize::WORD;
            case Register::R12W: return OperandSize::WORD;
            case Register::R13W: return OperandSize::WORD;
            case Register::R14W: return OperandSize::WORD;
            case Register::R15W: return OperandSize::WORD; 
            case Register::EAX:  return OperandSize::DWORD;
            case Register::EBX:  return OperandSize::DWORD;
            case Register::ECX:  return OperandSize::DWORD;
            case Register::EDX:  return OperandSize::DWORD;
            case Register::ESI:  return OperandSize::DWORD;
            case Register::EDI:  return OperandSize::DWORD;
            case Register::EBP:  return OperandSize::DWORD;
            case Register::ESP:  return OperandSize::DWORD;
            case Register::R8D:  return OperandSize::DWORD; 
            case Register::R9D:  return OperandSize::DWORD;
            case Register::R10D: return OperandSize::DWORD;
            case Register::R11D: return OperandSize::DWORD;
            case Register::R12D: return OperandSize::DWORD;
            case Register::R13D: return OperandSize::DWORD;
            case Register::R14D: return OperandSize::DWORD;
            case Register::R15D: return OperandSize::DWORD; 
            case Register::RAX:  return OperandSize::QWORD;
            case Register::RBX:  return OperandSize::QWORD;
            case Register::RCX:  return OperandSize::QWORD;
            case Register::RDX:  return OperandSize::QWORD;
            case Register::RSI:  return OperandSize::QWORD;
            case Register::RDI:  return OperandSize::QWORD;
            case Register::RBP:  return OperandSize::QWORD;
            case Register::RSP:  return OperandSize::QWORD; 
            case Register::R8:   return OperandSize::QWORD; 
            case Register::R9:   return OperandSize::QWORD; 
            case Register::R10:  return OperandSize::QWORD;
            case Register::R11:  return OperandSize::QWORD;
            case Register::R12:  return OperandSize::QWORD;
            case Register::R13:  return OperandSize::QWORD;
            case Register::R14:  return OperandSize::QWORD;
            case Register::R15:  return OperandSize::QWORD;
        }
        std::cout << "warning: Unknown size for register '" + to_string(_register) + "'\n";
        return OperandSize::BYTE;
    }

    static inline Register get_lower_byte(Register _reg)
    {
        switch (_reg)
        {
            case Register::AL:
            case Register::AH:
            case Register::AX:
            case Register::EAX:
            case Register::RAX: return Register::AL;
            case Register::BL:
            case Register::BH:
            case Register::BX:
            case Register::EBX:
            case Register::RBX: return Register::BL;
            case Register::CL:
            case Register::CH:
            case Register::CX:
            case Register::ECX:
            case Register::RCX: return Register::CL;
            case Register::DL:
            case Register::DH:
            case Register::DX:
            case Register::EDX:
            case Register::RDX: return Register::DL;
            case Register::SIL:
            case Register::SI:
            case Register::ESI:
            case Register::RSI: return Register::SIL;
            case Register::DIL:
            case Register::DI:
            case Register::EDI:
            case Register::RDI: return Register::DIL;
            case Register::BPL: 
            case Register::BP:
            case Register::EBP:
            case Register::RBP: return Register::BPL;
            case Register::SPL:
            case Register::SP:
            case Register::ESP:
            case Register::RSP: return Register::SPL;
            case Register::R8B:
            case Register::R8W:
            case Register::R8D:
            case Register::R8:  return Register::R8B;
            case Register::R9B:
            case Register::R9W:
            case Register::R9D:
            case Register::R9:  return Register::R9B;
            case Register::R10B:
            case Register::R10W:
            case Register::R10D:
            case Register::R10:  return Register::R10B;
            case Register::R11B:
            case Register::R11W:
            case Register::R11D:
            case Register::R11:  return Register::R11B;
            case Register::R12B:
            case Register::R12W:
            case Register::R12D:
            case Register::R12:  return Register::R12B;
            case Register::R13B:
            case Register::R13W:
            case Register::R13D:
            case Register::R13:  return Register::R13B;
            case Register::R14B:
            case Register::R14W:
            case Register::R14D:
            case Register::R14:  return Register::R14B;
            case Register::R15B:
            case Register::R15W:
            case Register::R15D:
            case Register::R15:  return Register::R15B;
        }
        std::cerr << "error: Cannot get lower byte for register '" << to_string(_reg) << "'\n";
        return Register::AL;
    }
};