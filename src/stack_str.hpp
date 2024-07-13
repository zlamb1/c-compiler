#pragma once

#include <exception>
#include <string_view>

class PopException : public std::exception
{
};

class PeekException : public std::exception
{
};

class ReverseException : public std::exception
{
};

#include <iostream>

class StackString
{
    public:
        StackString(std::string_view view) : m_View(view) 
        {
        }        

        size_t getCapacity() const; 
        bool hasCapacity() const; 
        bool hasCapacity(size_t len) const; 

        char pop(); 
        char peek() const;
        std::string_view peek(size_t len) const;  
        void reverse(); 

        void reset();

    private:
        std::string_view m_View; 
        size_t m_Position = 0; 
};