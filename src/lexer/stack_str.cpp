#include "stack_str.hpp"

size_t StackString::getCapacity() const
{
    return m_View.length() - m_Position;
}

bool StackString::hasCapacity() const
{
    return hasCapacity(1); 
}

bool StackString::hasCapacity(size_t len) const
{
    return getCapacity() >= len; 
} 

#include <iostream>

char StackString::pop()
{
    if (m_View.length() - m_Position < 1)
        throw PopException(); 
    char c = m_View[m_Position]; 
    m_Position++; 
    return c; 
}

char StackString::peek() const
{
    if (m_Position >= m_View.length())
        throw PeekException(); 
    return m_View[m_Position]; 
}

std::string_view StackString::peek(size_t len) const
{
    if (len == 0 || m_Position + len > m_View.length())
        throw PeekException(); 
    return m_View.substr(m_Position, m_Position + len); 
}

void StackString::reverse()
{
    if (m_Position <= 0)
        throw ReverseException();
    m_Position--; 
}

void StackString::reset()
{
    m_Position = 0; 
}