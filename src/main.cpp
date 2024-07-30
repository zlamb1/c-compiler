#include <cstring>
#include <memory>

#include "compiler/compiler.hpp"

const std::string TEST_FILE = "wrong_case.c";

int main(int argc, char *argv[])
{
    std::string filepath = "./examples/stage_1/invalid/" + TEST_FILE; 
    Compiler compiler = Compiler(filepath, argc, argv); 
    compiler.CompileProgram(); 
}