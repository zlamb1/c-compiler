#pragma once

#include <string>

struct CompilerFlags
{
    std::string filepath;
    std::string outputpath = "a.out"; 
    bool output_asm = false; 
};