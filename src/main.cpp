#include <cstring>
#include <memory>

#include "compiler/compiler.hpp"

const std::string TEST_FILE = "test.c";

int main(int argc, char *argv[])
{
    std::string filepath = "./examples/exclude/" + TEST_FILE; 
    Compiler compiler = Compiler(filepath, argc, argv); 
    compiler.CompileProgram(); 
}