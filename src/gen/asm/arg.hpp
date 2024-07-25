#pragma once

#include <memory>

#include "register.hpp"

enum class ArgType : int
{
    Register,
    Pointer,
    Immediate,
    Label
};

class AssemblyArg
{
public:
    AssemblyArg(ArgType type) : _type(type)
    {
    }

    virtual ~AssemblyArg() = default; 

    ArgType type() const
    {
        return _type; 
    }

    virtual bool is_eax() const { return false; }

    typedef std::shared_ptr<AssemblyArg> Ref; 
    
    template<typename T>
    static std::shared_ptr<T> RefCast(AssemblyArg::Ref ref)
    {
        return std::dynamic_pointer_cast<T>(ref); 
    }

private:
    ArgType _type; 
};

class RegisterArg : public AssemblyArg
{
public:
    Register _register; 

    RegisterArg(Register _register) : AssemblyArg(ArgType::Register), _register(_register)
    {
    }

    virtual ~RegisterArg() = default;
    
    bool is_eax() const override
    {
        return _register == Register::EAX;
    }

    typedef std::shared_ptr<RegisterArg> Ref; 

protected:
    RegisterArg(ArgType type, Register _register) : AssemblyArg(type), _register(_register)
    {
    } 
};

struct PointerArg : public RegisterArg
{
    int offset;

    PointerArg(Register _register, int offset) : RegisterArg(ArgType::Pointer, _register), offset(offset)
    {
    }

    bool is_eax() const override
    {
        return _register == Register::EAX && offset == 0; 
    }

    typedef std::shared_ptr<PointerArg> Ref; 
};

struct ImmediateArg : public AssemblyArg
{
    ImmediateArg(int value) : AssemblyArg(ArgType::Immediate), value(value)
    {
    }

    int value;
};

struct LabelArg : public AssemblyArg
{
    LabelArg(const std::string& label) : AssemblyArg(ArgType::Label), label(label)
    {
    }

    std::string label;
};