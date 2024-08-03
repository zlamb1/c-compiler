#pragma once

#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "symbol.hpp"

struct Scope
{
    std::unordered_map<std::string, VarSymbol::Ref> symbols; 
    std::string start_label, end_label;

    Scope() : symbols()
    {
    }

    Scope(const std::string& start_label, const std::string& end_label) : symbols(), start_label(start_label), end_label(end_label)
    {
    }
};

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

    void push_scope(const std::string& start_label, const std::string& end_label)
    {
        scope_stack.emplace_back(Scope(start_label, end_label));
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
        if (scope.symbols.find(var_name) != scope.symbols.end())
            throw std::runtime_error("error: Redeclaration of identifier '" + var_name + "'"); 
        else get_scope().symbols[var_name] = var_symbol;
    }

    VarSymbol::Ref get_var(const std::string& var_name)
    {
        // traverse vector in reverse
        for (size_t i = scope_stack.size() - 1; i < scope_stack.size(); i--)
        {
            auto scope = scope_stack[i];
            if (scope.symbols.find(var_name) != scope.symbols.end())
                return scope.symbols[var_name]; 
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

    const std::string& get_start_label() const
    {
        for (size_t i = scope_stack.size() - 1; i < scope_stack.size(); i--)
        {
            auto& scope = scope_stack[i]; 
            if (!scope.start_label.empty())
                return scope.start_label;
        }
        throw std::runtime_error("error: continue statement not within loop");
    }

    const std::string& get_end_label() const
    {
        for (size_t i = scope_stack.size() - 1; i < scope_stack.size(); i--)
        {
            auto& scope = scope_stack[i]; 
            if (!scope.end_label.empty())
                return scope.end_label;
        }
        throw std::runtime_error("error: break statement not within loop or switch");
    }

    bool any_scope_has_var(const std::string& var_name) const
    {
        // traverse vector in reverse
        for (size_t i = scope_stack.size() - 1; i >= 0; i--)
        {
            auto scope = scope_stack[i];
            if (scope.symbols.find(var_name) != scope.symbols.end())
                return true;
        }
        return false;
    }

    bool current_scope_has_var(const std::string& var_name) const
    {
        auto scope = get_scope();
        return scope.symbols.find(var_name) != scope.symbols.end(); 
    }
    
private:
    std::vector<Scope> scope_stack;
};