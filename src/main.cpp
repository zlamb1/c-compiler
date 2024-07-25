#include <cstring>
#include <memory>

#include "compiler/compiler.hpp"

const std::string STAGE_NUM = "stage3";
const std::string TEST_FILE = "div.c";

int main(int argc, char *argv[])
{
    std::string filepath = "./examples/" + STAGE_NUM + "/" + TEST_FILE; 
    Compiler compiler = Compiler(filepath, argc, argv); 
    compiler.CompileProgram(); 
}