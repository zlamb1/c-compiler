#pragma once

#include <stack>
#include <string>
#include <unordered_map>

#include "arg.hpp"
#include "code_gen.hpp"
#include "instruction.hpp"
#include "register.hpp"
#include "reg_alloc.hpp"

#include "gen/context.hpp"
#include "ir/tac.hpp"

class VarLocation
{
public:
    int stack_offset; 
    Register _register;

    enum class Type
    {
        Stack,
        Register,
        None
    };

    VarLocation() : _type(Type::None) {}

    VarLocation(int stack_offset) : _type(Type::Stack), stack_offset(stack_offset)
    {
    }

    VarLocation(Register _register) : _type(Type::Register), _register(_register)
    {
    }

    Type type() const { return _type; }

private:
    Type _type;
};

class ASMGenerator
{
public:
    ASMGenerator(ASMCodeGenerator& codeGenerator) : m_CodeGenerator(codeGenerator)
    {
    }

    void GenerateAssembly(TACGenerator& generator);
    void GenerateFunction(VarContext& var_context, TAC::Function::Ref function);
    void GenerateTriple(VarContext& var_context, TAC::TripleStatement::Ref triple);
    void GenerateQuad(VarContext& var_context, TAC::QuadStatement::Ref quad); 

private: 
    ASMCodeGenerator& m_CodeGenerator; 
    RegisterAllocator allocator{}; 
    int m_StackIndex = 0; 

    std::unordered_map<VarSymbol::Ref, VarLocation> m_LocationTable;

    void AllocIfTemp(VarContext& var_context, VarSymbol::Ref var_symbol)
    {
        if (var_symbol->is_temp && m_LocationTable.find(var_symbol) == m_LocationTable.end()) 
            m_LocationTable[var_symbol] = VarLocation(allocator.AllocRegister()); 
    }

    inline bool IsRegister(AssemblyArg::Ref arg, Register _register)
    {
        return arg->type() == ArgType::Register && AssemblyArg::RefCast<RegisterArg>(arg)->_register == _register; 
    }

    inline bool IsSameRegister(AssemblyArg::Ref arg1, AssemblyArg::Ref arg2)
    {
        if (arg1->type() != arg2->type()) return false;
        switch (arg1->type())
        {
            case ArgType::Immediate:
                return AssemblyArg::RefCast<ImmediateArg>(arg1)->value ==
                       AssemblyArg::RefCast<ImmediateArg>(arg2)->value;
            case ArgType::Label:
                return AssemblyArg::RefCast<LabelArg>(arg1)->label ==
                       AssemblyArg::RefCast<LabelArg>(arg2)->label;
            case ArgType::Pointer:
            {
                auto p1 = AssemblyArg::RefCast<PointerArg>(arg1);
                auto p2 = AssemblyArg::RefCast<PointerArg>(arg2);
                return p1->_register == p2->_register && p1->offset == p2->offset; 
            }
            case ArgType::Register:
                return AssemblyArg::RefCast<RegisterArg>(arg1)->_register ==
                       AssemblyArg::RefCast<RegisterArg>(arg2)->_register;

        }
        return false;
    }

    static inline bool IsImmediate(AssemblyArg::Ref arg)
    {
        return arg->type() == ArgType::Immediate;
    }

    static inline bool IsPointer(AssemblyArg::Ref arg)
    {
        return arg->type() == ArgType::Pointer; 
    }

    AssemblyArg::Ref FetchVarLocation(VarContext& var_context, VarSymbol::Ref var_symbol)
    {
        auto loc = m_LocationTable[var_symbol];
        if (var_symbol->is_temp) return CreateRef<RegisterArg>(loc._register); 
        else return CreateRef<PointerArg>(Register::RBP, loc.stack_offset); 
    }

    AssemblyArg::Ref FetchVarLocation(TAC::Operand::Ref operand)
    {
        if (operand->type() == TAC::OperandType::Constant)
            return CreateRef<ImmediateArg>(operand->get_value());
        else {
            auto loc = m_LocationTable[operand->get_symbol()];
            if (loc.type() == VarLocation::Type::Register)
                return CreateRef<RegisterArg>(loc._register); 
            else return CreateRef<PointerArg>(Register::RBP, loc.stack_offset); 
        }
    }
};