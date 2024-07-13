#include "lexer.hpp"

int main()
{
    Lexer lexer{}; 
    auto tokens = lexer.LexFile("./examples/stage1/return_2.c");

    if (tokens.size())
    {
        std::cout << std::endl; 
        std::cout << "<------------- Lexical Analysis ------------->" << std::endl; 
        for (auto token : tokens)
        {
            auto value = token.value.empty() ? "No Value" : token.value; 
            std::cout << "Token { " << (int) token.kind << ", " << value << " }" << std::endl;
        }
    } else {
        std::cout << "No tokens were lexed." << std::endl; 
    }
}