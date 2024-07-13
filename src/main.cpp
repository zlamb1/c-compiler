#include "gen/code_gen.hpp"

#include "lexer/lexer.hpp"

#include "parser/rd_parser.hpp"
#include "parser/print.hpp"

static std::string STAGE_NUM = "stage1";
static std::string TEST_FILE = "hex.c";

static std::vector<std::string> split(const std::string& str, const std::string& del)
{
    std::vector<std::string> found{}; 
    int start, end = -1 * del.size();
    do {
        start = end + del.size();
        end = str.find(del, start);
        found.emplace_back(str.substr(start, end - start)); 
    } while (end != -1);
    return found;
}

int main(int argc, char *argv[])
{
    std::string filepath = "./examples/" + STAGE_NUM + "/" + TEST_FILE; 
    if (argc > 1)
    {
        filepath = argv[1]; 
    }

    Lexer lexer{}; 
    auto tokens = lexer.LexFile(filepath);

    RDParser parser{ tokens }; 
    auto ast = parser.parse(); 

    pretty_print_ast(ast); 

    GenInfo genInfo{ "program" };
    if (filepath.substr(filepath.length() - 2) == ".c")
        genInfo.name = filepath.substr(0, filepath.length() - 2);

    genCode(ast, genInfo); 
}