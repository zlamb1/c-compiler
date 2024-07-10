#include "lexer.hpp"

int main()
{
    Lexer lexer{}; 
    auto tokens = lexer.LexFile("./examples/00.lang");

    if (tokens.size())
    {
        for (auto token : tokens)
        {
            std::cout << "Token Value: " << token.value << std::endl;
        }
    } else {
        std::cout << "No tokens were lexed." << std::endl; 
    }
}