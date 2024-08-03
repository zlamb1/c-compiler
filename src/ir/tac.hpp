#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "convert.hpp"
#include "context.hpp"
#include "symbol.hpp"
#include "types.hpp"

// Three-Address Code Generator

class TACGenerator
{
public:
    void GenerateStatements(AbstractSyntax::Ref root);
    void LogStatements() const; 

    const std::vector<TAC::Function::Ref>& GetFunctions() const { return m_Functions; }
    VarContext& GetVarContext() { return m_VarContext; }
protected:
    TAC::Function::Ref m_Function;
    std::vector<TAC::Function::Ref> m_Functions; 
    VarContext m_VarContext;

    inline VarSymbol::Ref CreateTempVar();
    inline std::string CreateLabel();
    inline TAC::Operand::Ref CreateOperand(AbstractSyntax::Ref syntax);

    void EvaluateSyntax(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax, VarSymbol::Ref dst);

    inline size_t GetStatementsSize()
    {
        assert(m_Function); 
        return m_Function->statements.size(); 
    }

    inline void SetScopeLabels(AbstractSyntax::Ref syntax, const std::string& start_label, const std::string& end_label)
    {
        if (syntax->type() == SyntaxType::CompoundBlock)
        {
            auto block = AbstractSyntax::RefCast<CompoundBlock>(syntax);
            block->start_label = start_label;
            block->end_label = end_label;
        }
    }

    inline void AddStatement(TAC::Statement::Ref statement)
    {
        assert(m_Function);
        m_Function->statements.emplace_back(statement); 
    }

    inline void UpdateRange(VarSymbol::Ref var_symbol)
    {
        assert(m_Function);
        var_symbol->range.end = GetStatementsSize();
    }
    
    inline void UpdateRange(TAC::Operand::Ref operand)
    {
        assert(m_Function);
        if (operand->type() == TAC::OperandType::Symbol)
            operand->get_symbol()->range.end = GetStatementsSize();
    }
};