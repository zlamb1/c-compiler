#pragma once

#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "symbol.hpp"

typedef std::unordered_map<std::string, VarSymbol::Ref> Scope;

class VarContext
{
public:
    VarContext()
    {
        // global scope
        push_scope();
    }

    void push_scope()
    {
        scope_stack.emplace_back(Scope());
    }

    void pop_scope()
    {
        if (scope_stack.size() < 1)
            throw std::runtime_error("error: Cannot remove stack from empty context");
        scope_stack.pop_back();
    }

    void add_var(const std::string& var_name, VarSymbol::Ref var_symbol)
    {
        // check if var already exists in top scope
        auto scope = get_scope();
        if (scope.find(var_name) != scope.end())
            throw std::runtime_error("error: Redeclaration of identifier '" + var_name + "'"); 
        else get_scope()[var_name] = var_symbol;
    }

    VarSymbol::Ref get_var(const std::string& var_name)
    {
        // traverse vector in reverse
        for (size_t i = scope_stack.size() - 1; i >= 0; i--)
        {
            auto scope = scope_stack[i];
            if (scope.find(var_name) != scope.end())
                return scope[var_name]; 
        }
        // no variable found
        throw std::runtime_error("error: Undeclared identifier '" + var_name + "'"); 
    }

    const VarSymbol::Ref get_var(const std::string& var_name) const
    {
        return get_var(var_name);
    }

    Scope& get_scope()
    {
        if (scope_stack.size() < 1)
            throw std::runtime_error("error: Cannot get scope from empty context");
        return scope_stack[scope_stack.size() - 1]; 
    }

    const Scope& get_scope() const 
    {
        if (scope_stack.size() < 1)
            throw std::runtime_error("error: Cannot get scope from empty context");
        return scope_stack[scope_stack.size() - 1]; 
    }

    bool any_scope_has_var(const std::string& var_name) const
    {
        // traverse vector in reverse
        for (size_t i = scope_stack.size() - 1; i >= 0; i--)
        {
            auto scope = scope_stack[i];
            if (scope.find(var_name) != scope.end())
                return true;
        }
        return false;
    }

    bool current_scope_has_var(const std::string& var_name) const
    {
        auto scope = get_scope();
        return scope.find(var_name) != scope.end(); 
    }
    
private:
    std::vector<Scope> scope_stack;
};