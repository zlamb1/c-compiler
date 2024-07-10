#include "lexer.hpp"

std::vector<Token> Lexer::LexFile(const std::string& filepath)
{
    std::ifstream lexfile;
    lexfile.open(filepath);

    if (lexfile.is_open())
    {
        std::vector<std::string> lines; 
        std::string line;
        while (std::getline(lexfile, line))
        {
            lines.emplace_back(line); 
        }

        return LexLines(lines); 
    } else
    {
        std::cout << "Could not open file!" << std::endl;
    }

    return {}; 
}

std::vector<Token> Lexer::LexLines(const std::vector<std::string>& lines)
{
    return {}; 
}