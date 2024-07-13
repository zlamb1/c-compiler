#include "lexer/lexer.hpp"

static std::string STAGE_NUM = "stage1";
static std::string TEST_FILE = "return_0.c";

int main()
{
    Lexer lexer{}; 
    auto tokens = lexer.LexFile("./examples/" + STAGE_NUM + "/" + TEST_FILE);

    if (tokens.size())
    {
        std::cout << std::endl; 
        std::cout << "<------------- Lexical Analysis ------------->" << std::endl; 
        for (auto token : tokens)
        {
            auto value = token.value.empty() ? "No Value" : token.value; 
            std::cout << TOKEN_KIND_NAMES[token.kind] << " : " << value << std::endl;
        }
    } else std::cout << "No tokens were lexed." << std::endl; 
}