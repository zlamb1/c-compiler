#pragma once

#include <math.h>
#include <unordered_map>

static std::unordered_map<char, int> HEX_DIGITS
{
    { '0',  0 },
    { '1',  1 },
    { '2',  2 },
    { '3',  3 },
    { '4',  4 },
    { '5',  5 },
    { '6',  6 },
    { '7',  7 },
    { '8',  8 },
    { '9',  9 },
    { 'a', 10 },
    { 'A', 10 },
    { 'b', 11 },
    { 'B', 11 },
    { 'c', 12 },
    { 'C', 12 },
    { 'd', 13 },
    { 'D', 13 },
    { 'e', 14 },
    { 'E', 14 },
    { 'f', 15 },
    { 'F', 15 }
};

// handles regular constants, octal numbers, and hex
static int parse_c_int(const std::string& str)
{
    if (str.length() > 1)
    {
        // hex number
        if (str.substr(0, 2) == "0x")
        {
            auto hex = str.substr(2); 
            int accumulator = 0;
            for (size_t i = 0; i < hex.length(); i++)
            {
                int digit = HEX_DIGITS[hex[i]];
                accumulator += pow(16, hex.length() - i - 1) * digit; 
            }
            return accumulator; 
        }
        
        // octal number
        if (str[0] == '0')
        {
            int accumulator = 0; 
            for (size_t i = 1; i < str.length(); i++)
            {
                int digit = str[i] - '0'; 
                accumulator += pow(8, str.length() - i - 1) * digit; 
            }
            return accumulator; 
        }
    }

    return std::stoi(str); 
}