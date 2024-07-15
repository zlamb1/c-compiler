#include <cstring>
#include <memory>

#include "compiler.hpp"

#include "gen/code_gen.hpp"

#include "lexer/lexer.hpp"

#include "parser/rd_parser.hpp"
#include "parser/print.hpp"

const std::string STAGE_NUM = "stage2";
const std::string TEST_FILE = "neg.c";

int main(int argc, char *argv[])
{
    std::string filepath = "./examples/" + STAGE_NUM + "/" + TEST_FILE; 
    auto lexer = Lexer();     
    auto parser = RDParser(lexer); 
    Compiler compiler = Compiler(filepath, argc, argv, parser); 
    compiler.CompileProgram(); 
}