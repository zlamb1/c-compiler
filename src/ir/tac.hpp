#pragma once

#include <cassert>
#include <iostream>
#include <vector>

#include "convert.hpp"
#include "types.hpp"

// Three-Address Code Generator

class TACGenerator
{
public:
    void GenerateStatements(AbstractSyntax::Ref root);
    void LogStatements() const; 
protected:
    std::vector<TAC::Statement::Ref> m_Statements; 
    size_t m_LabelCounter = 0, m_TempCounter = 0;

    inline VariableRef::Ref CreateTempVar();
    inline std::string CreateLabel();
    inline TAC::Operand::Ref CreateOperand(AbstractSyntax::Ref syntax);

    void EvaluateSyntax(AbstractSyntax::Ref syntax);
    TAC::Operand::Ref EvaluateExpression(AbstractSyntax::Ref syntax);
};