#pragma once

#include <exception>

class UndeclaredException : public std::runtime_error
{
public:
    UndeclaredException(const std::string& variableName) : std::runtime_error(std::string("error: Unknown identifier '" + variableName + "'"))
    {
    }
};

class RedeclaredException : public std::runtime_error
{
public:
    RedeclaredException(const std::string& variableName) : std::runtime_error(std::string("error: The identifier '" + variableName + "' has already been declared"))
    {
    } 
};