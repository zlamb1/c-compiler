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
    m_LinePosition++;
    if (c == '\n') 
    {
        m_LinePosition = 1; 
        m_Line++; 
    }
    m_Position++; 
    return c; 
}

std::string_view StackString::pop(size_t len)
{
    if (len < 1 || m_View.length() - m_Position < len)
        throw PopException(); 
    auto view = m_View.substr(m_Position, len);
    m_LinePosition += len; 
    for (char c : view)
    {
        if (c == '\n') 
        {
            m_LinePosition = 1; 
            m_Line++; 
        }
    }
    m_Position += len; 
    return view; 
}

char StackString::peek() const
{
    if (m_Position >= m_View.length())
        throw PeekException(); 
    return m_View[m_Position]; 
}

std::string_view StackString::peek(size_t len) const
{
    if (len < 1 || m_Position + len > m_View.length())
        throw PeekException(); 
    return m_View.substr(m_Position, len); 
}

void StackString::reset()
{
    m_Position = 0; 
    m_Line = 1; 
}