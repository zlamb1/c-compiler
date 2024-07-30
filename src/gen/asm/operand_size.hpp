#pragma once

#include <assert.h>

enum class OperandSize : int
{
    QWORD,
    DWORD,
    WORD,
    BYTE
};

namespace OperandUtility
{
    static inline std::string to_string(OperandSize operand_size)
    {
        switch (operand_size)
        {
            case OperandSize::BYTE:  return "b";
            case OperandSize::WORD:  return "w";
            case OperandSize::DWORD: return "l"; 
            case OperandSize::QWORD: return "q"; 
        }
        assert(false);
        return std::string(); 
    }
}