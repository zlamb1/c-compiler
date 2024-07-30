#include "tac.hpp"

void TACGenerator::GenerateStatements(AbstractSyntax::Ref root)
{
    m_LabelCounter = 0, m_TempCounter = 0;
    m_Statements.clear(); 
    EvaluateSyntax(root); 
}

void TACGenerator::LogStatements() const
{
    bool logLineNumber = true; 
    std::string prefix = "";
    size_t counter = 0;
    for (auto statement : m_Statements)
    {
        if (logLineNumber) prefix = std::to_string(counter) + ": ";
        counter++;
        switch (statement->type())
        {
            case TAC::StatementType::Goto:
            {
                auto _goto = TAC::Statement::RefCast<TAC::GotoStatement>(statement);
                std::cout << prefix << "goto " << _goto->label << "\n";
                break;
            }
            case TAC::StatementType::Condition:
            {
                auto cond = TAC::Statement::RefCast<TAC::ConditionStatement>(statement); 
                std::cout << prefix << "if (" << cond->condition->to_string() << ") goto " << cond->goto_label << "\n";
                break;
            }
            case TAC::StatementType::Assign:
            {
                auto assign = TAC::Statement::RefCast<TAC::AssignStatement>(statement); 
                auto lhs = assign->lhs->name; 
                auto rhs = assign->rhs->to_string();
                std::cout << prefix << lhs << " = " << rhs << "\n"; 
                break;
            }
            case TAC::StatementType::Triple:
            {
                auto triple = TAC::Statement::RefCast<TAC::TripleStatement>(statement);
                auto rhs = triple->rhs->to_string(); 
                auto result = triple->result;
                std::cout << prefix << result->name << " = " << TAC::OP_CODE_SYMBOLS[triple->op] << rhs << "\n";
                break;
            }
            case TAC::StatementType::Quad:
            {
                auto quad = TAC::Statement::RefCast<TAC::QuadStatement>(statement);
                auto lhs = quad->lhs->to_string();
                auto rhs = quad->rhs->to_string();
                std::cout << prefix << quad->result->name << " = " << lhs 
                    << " " << TAC::OP_CODE_SYMBOLS[quad->op] << " " << rhs << "\n"; 
                break;
            }
            case TAC::StatementType::Label:
            {
                auto label = TAC::Statement::RefCast<TAC::LabelStatement>(statement);
                std::cout << prefix << label->name << ":\n";
                break;
            }
            case TAC::StatementType::Return:
            {
                auto ret = TAC::Statement::RefCast<TAC::ReturnStatement>(statement);
                auto ret_val = ret->ret_val->to_string();
                std::cout << prefix << "RET " << ret_val << "\n"; 
                break;
            }
        }
    }
}    

VariableRef::Ref TACGenerator::CreateTempVar()
{
    auto name = "t" + std::to_string(++m_TempCounter);
    // find unsued name
    while (m_SymbolTable.find(name) != m_SymbolTable.end())
        name = "t" + std::to_string(++m_TempCounter); 
    auto symbol = VarSymbol(name, 4); 
    symbol.is_temp = true;
    symbol.range = VarRange(m_Statements.size());
    m_SymbolTable[name] = symbol; 
    return CreateRef<VariableRef>(name);
}

std::string TACGenerator::CreateLabel()
{
    return "L" + std::to_string(++m_LabelCounter); 
}

TAC::Operand::Ref TACGenerator::CreateOperand(AbstractSyntax::Ref syntax)
{
    if (syntax->type() == SyntaxType::IntConstant)
        return CreateRef<TAC::Operand>(AbstractSyntax::RefCast<IntConstant>(syntax));
    else return EvaluateExpression(syntax);
}

void TACGenerator::EvaluateSyntax(AbstractSyntax::Ref syntax)
{
    switch (syntax->type())
    {
        case SyntaxType::Program:
            EvaluateSyntax(AbstractSyntax::RefCast<Program>(syntax)->function); 
            break;
        case SyntaxType::Function:
            EvaluateSyntax(AbstractSyntax::RefCast<Function>(syntax)->block); 
            break;
        case SyntaxType::StatementExpression:
            EvaluateExpression(AbstractSyntax::RefCast<StatementExpression>(syntax)->expr); 
            break;
        case SyntaxType::AssignmentOp:
        {
            auto rhs = EvaluateExpression(AbstractSyntax::RefCast<AssignmentOp>(syntax)->rvalue); 
            break;
        }
        case SyntaxType::Declaration:
        {
            auto decl = AbstractSyntax::RefCast<Declaration>(syntax);
            for (auto var : decl->variables)
            {
                auto lhs = CreateRef<VariableRef>(var.name); 
                auto rhs = EvaluateExpression(var.expr, lhs); 
                auto assign = CreateRef<TAC::AssignStatement>(lhs, rhs); 
                auto symbol = VarSymbol(var.name, 4);
                symbol.range = VarRange(m_Statements.size());
                m_SymbolTable[var.name] = symbol; 
            }
            break;
        }
        case SyntaxType::CompoundBlock:            
            for (auto statement : AbstractSyntax::RefCast<CompoundBlock>(syntax)->statements)
                EvaluateSyntax(statement); 
            break;
        case SyntaxType::IfStatement:
        {
            auto if_statement = AbstractSyntax::RefCast<IfStatement>(syntax);
            auto end_label = CreateLabel(); 
            auto if_label = CreateLabel(); 
            auto condition = EvaluateExpression(if_statement->if_conditional.condition);
            UpdateRange(condition);
            m_Statements.emplace_back(CreateRef<TAC::ConditionStatement>(condition, if_label));
            size_t len = if_statement->else_ifs.size(); 
            std::vector<std::string> labels;
            for (size_t i = 0; i < len; i++)
            {
                labels.emplace_back(CreateLabel()); 
                condition = EvaluateExpression(if_statement->else_ifs[i].condition);
                UpdateRange(condition); 
                m_Statements.emplace_back(CreateRef<TAC::ConditionStatement>(condition, labels[i]));
            }
            auto _else = if_statement->else_statement; 
            if (_else != nullptr) EvaluateSyntax(_else);
            m_Statements.emplace_back(CreateRef<TAC::GotoStatement>(end_label));
            for (size_t i = 0; i < len; i++)
            {
                m_Statements.emplace_back(CreateRef<TAC::LabelStatement>(labels[i]));
                EvaluateSyntax(if_statement->else_ifs[i].statement);
                m_Statements.emplace_back(CreateRef<TAC::GotoStatement>(end_label));
            }
            m_Statements.emplace_back(CreateRef<TAC::LabelStatement>(if_label));
            EvaluateSyntax(if_statement->if_conditional.statement);
            m_Statements.emplace_back(CreateRef<TAC::LabelStatement>(end_label));
            break;
        }
        case SyntaxType::Return:
        {
            auto ref = EvaluateExpression(AbstractSyntax::RefCast<Return>(syntax)->expr); 
            auto ret = CreateRef<TAC::ReturnStatement>(ref);
            m_Statements.emplace_back(ret);  
            break;
        }
        case SyntaxType::Assignment:
        {
            auto assignment = AbstractSyntax::RefCast<Assignment>(syntax);
            auto lhs = CreateRef<VariableRef>(assignment->lvalue);
            auto rhs = EvaluateExpression(assignment->rvalue);
            auto assign = CreateRef<TAC::AssignStatement>(lhs, rhs); 
            m_Statements.emplace_back(assign); 
            break;
        }
    }
}

TAC::Operand::Ref TACGenerator::EvaluateExpression(AbstractSyntax::Ref syntax)
{
    return EvaluateExpression(syntax, nullptr); 
}

TAC::Operand::Ref TACGenerator::EvaluateExpression(AbstractSyntax::Ref syntax, VariableRef::Ref dst)
{
    switch (syntax->type())
    {
        case SyntaxType::UnaryOp:
        {
            auto op = AbstractSyntax::RefCast<UnaryOp>(syntax);
            auto rhs = CreateOperand(op->expr); 
            UpdateRange(rhs);
            if (dst == nullptr) dst = CreateTempVar(); 
            auto triple = CreateRef<TAC::TripleStatement>(TAC::convert_unary_op(op->OpType()), rhs, dst); 
            m_Statements.emplace_back(triple);
            return CreateRef<TAC::Operand>(dst);
        }
        case SyntaxType::BinaryOp:
        {
            auto op = AbstractSyntax::RefCast<BinaryOp>(syntax);
            auto lhs = CreateOperand(op->lvalue);
            auto rhs = CreateOperand(op->rvalue);
            UpdateRange(lhs); 
            UpdateRange(rhs);
            if (op->OpType() == BinaryOpType::Comma)
                return rhs; 
            if (dst == nullptr) dst = CreateTempVar(); 
            auto quad = CreateRef<TAC::QuadStatement>(TAC::convert_binary_op(op->OpType()), 
                lhs, rhs, dst); 
            m_Statements.emplace_back(quad);
            return CreateRef<TAC::Operand>(dst); 
        }
        case SyntaxType::AssignmentOp:
        {
            auto op = AbstractSyntax::RefCast<AssignmentOp>(syntax); 
            auto lhs = CreateRef<VariableRef>(op->lvalue); 
            auto lhsOperand = CreateRef<TAC::Operand>(lhs);
            auto rhs = CreateOperand(op->rvalue);
            UpdateRange(lhs); 
            UpdateRange(rhs); 
            auto quad = CreateRef<TAC::QuadStatement>(TAC::convert_assignment_op(op->OpType()), 
                lhsOperand, rhs, lhs); 
            m_Statements.emplace_back(quad); 
            return lhsOperand; 
        }
        case SyntaxType::IntConstant:
        {
            auto constant = AbstractSyntax::RefCast<IntConstant>(syntax);
            if (dst != nullptr)
                m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, CreateRef<TAC::Operand>(constant)));
            return CreateRef<TAC::Operand>(constant); 
        }
        case SyntaxType::VariableRef:
        {
            auto ref = AbstractSyntax::RefCast<VariableRef>(syntax);
            UpdateRange(ref);
            return CreateRef<TAC::Operand>(ref);
        }
        case SyntaxType::Assignment:
        {
            auto assignment = AbstractSyntax::RefCast<Assignment>(syntax);
            auto lhs = CreateRef<VariableRef>(assignment->lvalue);
            auto rhs = CreateOperand(assignment->rvalue);
            UpdateRange(lhs);
            UpdateRange(rhs);
            auto assign = CreateRef<TAC::AssignStatement>(lhs, rhs); 
            m_Statements.emplace_back(assign); 
            return rhs;
        }
    }
    assert(false);
    return nullptr;
}