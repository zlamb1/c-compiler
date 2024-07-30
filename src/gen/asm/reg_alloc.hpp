#pragma once

#include <unordered_map>

#include "register.hpp"
#include "operand_size.hpp"

class RegisterAllocator
{
public:
    Register AllocRegister()
    {
        return AllocRegister<OperandSize::DWORD>(); 
    }

    template<OperandSize S>
    Register AllocRegister()
    {
        for (auto& iter : m_UsedRegisters)
        {
            if (RegisterUtility::get_register_size(iter.first) == S && IsRegisterFree(iter.first))
            {
                m_UsedRegisters[iter.first] = true; 
                return iter.first; 
            }
        }
    }

    void AllocRegister(Register _reg)
    {
        m_UsedRegisters[_reg] = true; 
    }

    void FreeRegister(Register _register)
    {
        m_UsedRegisters[_register] = false; 
    }

    void FreeRegisters()
    {
        for (auto& iter : m_UsedRegisters)
            m_UsedRegisters[iter.first] = false; 
    }

    bool IsRegisterFree(Register _register)
    {
        switch (_register)
        {
            case Register::AH:
                return !(m_UsedRegisters[Register::RAX] || m_UsedRegisters[Register::EAX] || m_UsedRegisters[Register::AX] ||
                    m_UsedRegisters[Register::AH]);
            case Register::AL:
                return !(m_UsedRegisters[Register::RAX] || m_UsedRegisters[Register::EAX] || m_UsedRegisters[Register::AX] ||
                    m_UsedRegisters[Register::AL]);
            case Register::AX:
            case Register::EAX:
            case Register::RAX:
                return !(m_UsedRegisters[Register::RAX] || m_UsedRegisters[Register::EAX] || m_UsedRegisters[Register::AX] ||
                    m_UsedRegisters[Register::AL] || m_UsedRegisters[Register::AH]);
            case Register::BH:
                return !(m_UsedRegisters[Register::RBX] || m_UsedRegisters[Register::EBX] || m_UsedRegisters[Register::BX] ||
                    m_UsedRegisters[Register::BH]);
            case Register::BL:
                return !(m_UsedRegisters[Register::RBX] || m_UsedRegisters[Register::EBX] || m_UsedRegisters[Register::BX] ||
                    m_UsedRegisters[Register::BL]);
            case Register::BX:
            case Register::EBX:
            case Register::RBX:
                return !(m_UsedRegisters[Register::RBX] || m_UsedRegisters[Register::EBX] || m_UsedRegisters[Register::BX] ||
                    m_UsedRegisters[Register::BL] || m_UsedRegisters[Register::BH]);
            case Register::CH:
                return !(m_UsedRegisters[Register::RCX] || m_UsedRegisters[Register::ECX] || m_UsedRegisters[Register::CX] ||
                    m_UsedRegisters[Register::CH]);
            case Register::CL:
                return !(m_UsedRegisters[Register::RCX] || m_UsedRegisters[Register::ECX] || m_UsedRegisters[Register::CX] ||
                    m_UsedRegisters[Register::CL]);
            case Register::CX:
            case Register::ECX:
            case Register::RCX:
                return !(m_UsedRegisters[Register::RCX] || m_UsedRegisters[Register::ECX] || m_UsedRegisters[Register::CX] ||
                    m_UsedRegisters[Register::CL] || m_UsedRegisters[Register::CH]);
            case Register::DH:
                return !(m_UsedRegisters[Register::RDX] || m_UsedRegisters[Register::EDX] || m_UsedRegisters[Register::DX] ||
                    m_UsedRegisters[Register::DH]);
            case Register::DL:
                return !(m_UsedRegisters[Register::RDX] || m_UsedRegisters[Register::EDX] || m_UsedRegisters[Register::DX] ||
                    m_UsedRegisters[Register::DL]);
            case Register::DX:
            case Register::EDX:
            case Register::RDX:
                return !(m_UsedRegisters[Register::RDX] || m_UsedRegisters[Register::EDX] || m_UsedRegisters[Register::DX] ||
                    m_UsedRegisters[Register::DL] || m_UsedRegisters[Register::DH]);
        }
        return false;
    }
private:
    std::unordered_map<Register, bool> m_UsedRegisters =
    {
        { Register::AH,   false },
        { Register::AL,   false },   
        { Register::BH,   false },   
        { Register::BL,   false },   
        { Register::CH,   false },   
        { Register::CL,   false },   
        { Register::DH,   false },   
        { Register::DL,   false },   
        { Register::SIL,  false },  
        { Register::BPL,  false },  
        { Register::SPL,  false },  
        { Register::R8B,  false },  
        { Register::R9B,  false },  
        { Register::R10B, false }, 
        { Register::R11B, false }, 
        { Register::R12B, false }, 
        { Register::R13B, false }, 
        { Register::R14B, false },  
        { Register::R15B, false }, 
        { Register::AX,   false },   
        { Register::BX,   false },   
        { Register::CX,   false },   
        { Register::DX,   false },   
        { Register::SI,   false },   
        { Register::DI,   false },   
        { Register::BP,   false },   
        { Register::SP,   false },   
        { Register::R8W,  false },  
        { Register::R9W,  false },  
        { Register::R10W, false }, 
        { Register::R11W, false }, 
        { Register::R12W, false }, 
        { Register::R13W, false }, 
        { Register::R14W, false }, 
        { Register::R15W, false },  
        { Register::EAX,  false },  
        { Register::EBX,  false },  
        { Register::ECX,  false },  
        { Register::EDX,  false },  
        { Register::ESI,  false },  
        { Register::EDI,  false },  
        { Register::EBP,  false },  
        { Register::ESP,  false },  
        { Register::R8D,  false },   
        { Register::R9D,  false },  
        { Register::R10D, false }, 
        { Register::R11D, false }, 
        { Register::R12D, false }, 
        { Register::R13D, false }, 
        { Register::R14D, false }, 
        { Register::R15D, false },  
        { Register::RAX,  false },  
        { Register::RBX,  false },  
        { Register::RCX,  false },  
        { Register::RDX,  false },  
        { Register::RSI,  false },  
        { Register::RDI,  false },  
        { Register::RBP,  false },  
        { Register::RSP,  false },   
        { Register::R8,   false },    
        { Register::R9,   false },    
        { Register::R10,  false },  
        { Register::R11,  false },  
        { Register::R12,  false },  
        { Register::R13,  false },  
        { Register::R14,  false },  
        { Register::R15,  false }
    };
};