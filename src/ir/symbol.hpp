#pragma once

#include <memory>
#include <string>

struct VarRange
{
    size_t start = 0, end = 0; 

    VarRange()
    {
    }

    VarRange(size_t start_and_end) : start(start_and_end), end(start_and_end)
    {
    }
};

struct VarSymbol
{
    std::string var_name; 
    size_t byte_size = 1; 
    bool is_temp = false; 
    VarRange range; 

    VarSymbol()
    {
    }

    VarSymbol(const std::string& var_name, size_t byte_size) : var_name(var_name), byte_size(byte_size), range()
    {
    }

    typedef std::shared_ptr<VarSymbol> Ref; 
};