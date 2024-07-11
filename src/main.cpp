#include "lexer.hpp"

int main()
{
    Lexer lexer{}; 
    auto tokens = lexer.LexFile("./examples/00.lang");

    if (tokens.size())
    {
        std::cout << std::endl; 
        std::cout << "<------------- Lexical Analysis ------------->" << std::endl; 
        for (auto token : tokens)
        {
            auto val = token.value.empty() ? "No Value" : token.value; 
            std::cout << "Token { " << TOKEN_KIND_NAMES[token.kind] << ", " << val << " }" << std::endl;
        }
    } else {
        std::cout << "No tokens were lexed." << std::endl; 
    }
}