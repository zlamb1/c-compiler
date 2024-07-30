#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "convert.hpp"
#include "symbol.hpp"
#include "types.hpp"

// Three-Address Code Generator

class TACGenerator
{
public:
    void GenerateStatements(AbstractSyntax::Ref root);
    void LogStatements() const; 

    const std::vector<TAC::Statement::Ref>& GetStatements() const { return m_Statements; }
    const std::unordered_map<std::string, VarSymbol>& GetSymbolTable() const { return m_SymbolTable; }
protected:
    std::vector<TAC::Statement::Ref> m_Statements; 
    std::unordered_map<std::string, VarSymbol> m_SymbolTable;
    size_t m_LabelCounter = 0, m_TempCounter = 0;

    inline VariableRef::Ref CreateTempVar();
    inline std::string CreateLabel();
    inline TAC::Operand::Ref CreateOperand(AbstractSyntax::Ref syntax);

    void EvaluateSyntax(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax, VariableRef::Ref dst);

    inline bool VarExists(const std::string& varName)
    {
        return m_SymbolTable.find(varName) != m_SymbolTable.end(); 
    }

    inline void CheckVarUndefined(const std::string& varName)
    {
        if (VarExists(varName))
            throw std::runtime_error("error: Redeclaration of identifier '" + varName + "'"); 
    }

    inline void CheckVarDefined(const std::string& varName)
    {
        if (!VarExists(varName))
            throw std::runtime_error("error: Unknown identifier '" + varName + "'");
    }

    inline void UpdateRange(VariableRef::Ref ref)
    {
        m_SymbolTable[ref->name].range.end = m_Statements.size();
    }
    
    inline void UpdateRange(TAC::Operand::Ref operand)
    {
        if (operand->type() == TAC::OperandType::Ref)
            m_SymbolTable[operand->get_name()].range.end = m_Statements.size();
    }
};