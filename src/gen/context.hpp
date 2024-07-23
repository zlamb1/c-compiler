#pragma once

#include <memory>
#include <unordered_map>

#include "utility/ref.hpp"

class BlockContext
{
public:
    BlockContext() 
    {
    }

    BlockContext(std::shared_ptr<BlockContext> parent) : m_StackIndex(parent->StackOffset()), m_ParentMap(parent->VariableMap())
    {
    }

    virtual ~BlockContext() = default; 

    bool HasVariable(const std::string& variableName)
    {
        return HasScopeVariable(variableName) || HasParentVariable(variableName);
    }

    bool HasScopeVariable(const std::string& variableName)
    {
        return m_VariableMap.find(variableName) != m_VariableMap.end(); 
    }

    bool HasParentVariable(const std::string& variableName)
    {
        return m_ParentMap.find(variableName) != m_ParentMap.end();
    }

    virtual int CreateVariable(const std::string& variableName, int offset)
    {
        auto index = m_StackIndex;
        m_VariableMap[variableName] = index;
        m_StackIndex -= offset;  
        return index;
    }
    
    virtual int GetVariableOffset(const std::string& variableName)
    {
        // prioritize current scope
        if (HasScopeVariable(variableName))
            return m_VariableMap[variableName];
        if (HasParentVariable(variableName))
            return m_ParentMap[variableName];
        // TODO: throw exc
        return 0;
    }

    int& StackOffset() { return m_StackIndex; } 
    const int& StackOffset() const { return m_StackIndex; }

    const std::unordered_map<std::string, int> VariableMap() const { return m_VariableMap; }

    typedef std::shared_ptr<BlockContext> Ref;
protected:
    // variable name : stack index
    std::unordered_map<std::string, int> m_VariableMap; 
    std::unordered_map<std::string, int> m_ParentMap;
    int m_StackIndex = 0;
};