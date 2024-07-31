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

    const std::vector<TAC::Statement::Ref>& GetStatements() const { return m_Statements; }
    VarContext& GetVarContext() { return m_VarContext; }
protected:
    std::vector<TAC::Statement::Ref> m_Statements; 
    VarContext m_VarContext;
    size_t m_LabelCounter = 0, m_TempCounter = 0;

    inline VarSymbol::Ref CreateTempVar();
    inline std::string CreateLabel();
    inline TAC::Operand::Ref CreateOperand(AbstractSyntax::Ref syntax);

    void EvaluateSyntax(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax, VarSymbol::Ref dst);

    inline void UpdateRange(VarSymbol::Ref var_symbol)
    {
        var_symbol->range.end = m_Statements.size();
    }
    
    inline void UpdateRange(TAC::Operand::Ref operand)
    {
        if (operand->type() == TAC::OperandType::Symbol)
            operand->get_symbol()->range.end = m_Statements.size();
    }
};