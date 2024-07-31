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
                auto lhs = assign->lhs->var_name; 
                auto rhs = assign->rhs->to_string();
                std::cout << prefix << lhs << " = " << rhs << "\n"; 
                break;
            }
            case TAC::StatementType::Triple:
            {
                auto triple = TAC::Statement::RefCast<TAC::TripleStatement>(statement);
                auto rhs = triple->rhs->to_string(); 
                auto dst = triple->dst;
                std::cout << prefix << dst->var_name << " = " << TAC::OP_CODE_SYMBOLS[triple->op] << rhs << "\n";
                break;
            }
            case TAC::StatementType::Quad:
            {
                auto quad = TAC::Statement::RefCast<TAC::QuadStatement>(statement);
                auto lhs = quad->lhs->to_string();
                auto rhs = quad->rhs->to_string();
                std::cout << prefix << quad->dst->var_name << " = " << lhs 
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

VarSymbol::Ref TACGenerator::CreateTempVar()
{
    auto name = "t" + std::to_string(++m_TempCounter);
    // find unsued name
    while (m_VarContext.current_scope_has_var(name))
        name = "t" + std::to_string(++m_TempCounter); 
    auto symbol = CreateRef<VarSymbol>(name, 4); 
    symbol->is_temp = true;
    symbol->range = VarRange(m_Statements.size());
    m_VarContext.add_var(name, symbol);
    return symbol;
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
                if (m_VarContext.current_scope_has_var(var.name))
                    throw std::runtime_error("error: Redeclaration of identifier '" + var.name + "'");
                auto lhs = CreateRef<VarSymbol>(var.name, 4);   
                m_VarContext.add_var(var.name, lhs); 
                auto rhs = var.expr ? EvaluateExpression(var.expr, lhs) : 
                    CreateRef<TAC::Operand>(CreateRef<IntConstant>(0)); 
                lhs->range = VarRange(m_Statements.size() - 1);
            }
            break;
        }
        case SyntaxType::CompoundBlock:    
            m_VarContext.push_scope();        
            for (auto statement : AbstractSyntax::RefCast<CompoundBlock>(syntax)->statements)
                EvaluateSyntax(statement);
            m_VarContext.pop_scope(); 
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
            if (_else != nullptr) 
                EvaluateSyntax(_else);
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
            auto lhs = m_VarContext.get_var(assignment->lvalue);
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

TAC::Operand::Ref TACGenerator::EvaluateExpression(AbstractSyntax::Ref syntax, VarSymbol::Ref dst)
{
    switch (syntax->type())
    {
        case SyntaxType::UnaryOp:
        {
            auto op = AbstractSyntax::RefCast<UnaryOp>(syntax);
            if (dst == nullptr) dst = CreateTempVar(); 
            switch (op->OpType())
            {
                case UnaryOpType::PostfixDecrement:
                case UnaryOpType::PostfixIncrement:
                {
                    auto rhs = CreateOperand(op->expr); 
                    auto op_code = op->OpType() == UnaryOpType::PostfixDecrement ? TAC::OpCode::SUB :
                        TAC::OpCode::ADD;
                    UpdateRange(rhs);
                    m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, rhs)); 
                    m_Statements.emplace_back(CreateRef<TAC::QuadStatement>(op_code, 
                        rhs, CreateRef<TAC::Operand>(CreateRef<IntConstant>(1)), rhs->get_symbol()));
                    return CreateRef<TAC::Operand>(dst);
                }
                case UnaryOpType::PrefixDecrement:
                case UnaryOpType::PrefixIncrement:
                {
                    auto rhs = CreateOperand(op->expr); 
                    auto op_code = op->OpType() == UnaryOpType::PrefixDecrement ? TAC::OpCode::SUB :
                        TAC::OpCode::ADD;
                    UpdateRange(rhs);
                    m_Statements.emplace_back(CreateRef<TAC::QuadStatement>(op_code, 
                        rhs, CreateRef<TAC::Operand>(CreateRef<IntConstant>(1)), rhs->get_symbol()));
                    m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, rhs)); 
                    return CreateRef<TAC::Operand>(dst);
                }
            }
            auto rhs = CreateOperand(op->expr); 
            UpdateRange(rhs);
            auto triple = CreateRef<TAC::TripleStatement>(TAC::convert_unary_op(op->OpType()), rhs, dst); 
            m_Statements.emplace_back(triple);
            return CreateRef<TAC::Operand>(dst);
        }
        case SyntaxType::BinaryOp:
        {
            auto op = AbstractSyntax::RefCast<BinaryOp>(syntax);
            auto lhs = CreateOperand(op->lvalue);
            UpdateRange(lhs); 
            switch (op->OpType())
            {
                case BinaryOpType::LogicalAnd:
                case BinaryOpType::LogicalOr:
                {
                    auto short_circuit_op = op->OpType() == BinaryOpType::LogicalAnd ? TAC::OpCode::EQL : TAC::OpCode::NEQL;
                    auto end = CreateLabel(); 
                    auto set = CreateLabel(); 
                    auto short_circuit_label = op->OpType() == BinaryOpType::LogicalAnd ? end : set; 
                    auto precond = CreateTempVar(); 
                    auto quad = CreateRef<TAC::QuadStatement>(short_circuit_op, 
                        CreateRef<TAC::Operand>(CreateRef<IntConstant>(0)), lhs, precond); 
                    m_Statements.emplace_back(quad);
                    m_Statements.emplace_back(CreateRef<TAC::ConditionStatement>(CreateRef<TAC::Operand>(precond), short_circuit_label));
                    auto rhs = CreateOperand(op->rvalue);
                    UpdateRange(rhs);
                    if (dst == nullptr) dst = CreateTempVar();
                    quad = CreateRef<TAC::QuadStatement>(TAC::OpCode::NEQL, 
                        CreateRef<TAC::Operand>(CreateRef<IntConstant>(0)), rhs, dst); 
                    m_Statements.emplace_back(quad);
                    m_Statements.emplace_back(CreateRef<TAC::GotoStatement>(end));
                    m_Statements.emplace_back(CreateRef<TAC::LabelStatement>(set));
                    m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, 
                        CreateRef<TAC::Operand>(CreateRef<IntConstant>(op->OpType() == BinaryOpType::LogicalAnd ? 0 : 1))));
                    m_Statements.emplace_back(CreateRef<TAC::LabelStatement>(end));
                    return CreateRef<TAC::Operand>(dst); 
                }
            }                
            auto rhs = CreateOperand(op->rvalue);
            UpdateRange(rhs);
            if (op->OpType() == BinaryOpType::Comma)
            {
                m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, rhs));
                return rhs; 
            }
            if (dst == nullptr) dst = CreateTempVar(); 
            auto quad = CreateRef<TAC::QuadStatement>(TAC::convert_binary_op(op->OpType()), 
                lhs, rhs, dst); 
            m_Statements.emplace_back(quad);
            return CreateRef<TAC::Operand>(dst); 
        }
        case SyntaxType::AssignmentOp:
        {
            auto op = AbstractSyntax::RefCast<AssignmentOp>(syntax); 
            auto lhs = m_VarContext.get_var(op->lvalue);
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
            auto symbol = m_VarContext.get_var(ref->name);
            UpdateRange(symbol);
            return CreateRef<TAC::Operand>(symbol);
        }
        case SyntaxType::Assignment:
        {
            auto assignment = AbstractSyntax::RefCast<Assignment>(syntax);
            auto lhs = m_VarContext.get_var(assignment->lvalue);
            auto rhs = CreateOperand(assignment->rvalue);
            UpdateRange(lhs);
            UpdateRange(rhs);
            auto assign = CreateRef<TAC::AssignStatement>(lhs, rhs); 
            m_Statements.emplace_back(assign); 
            if (dst != nullptr)
            {
                UpdateRange(lhs);
                m_Statements.emplace_back(CreateRef<TAC::AssignStatement>(dst, CreateRef<TAC::Operand>(lhs)));
            }
            return rhs;
        }
    }
    assert(false);
    return nullptr;
}