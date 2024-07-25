#include "generator.hpp"

void ASMGenerator::GenerateSyntax(AbstractSyntax::Ref syntax)
{
    switch (syntax->type())
    {
        case SyntaxType::Program:
            GenerateProgram(AbstractSyntax::RefCast<Program>(syntax)); 
            break;
        case SyntaxType::Function:
            GenerateFunction(AbstractSyntax::RefCast<Function>(syntax));
            break; 
        case SyntaxType::StatementExpression:
            GenerateStatementExpression(AbstractSyntax::RefCast<StatementExpression>(syntax));
            break;
        case SyntaxType::AssignmentOp:
            GenerateAssignmentOp(AbstractSyntax::RefCast<AssignmentOp>(syntax));
            break;
        case SyntaxType::Declaration:
            GenerateDeclaration(AbstractSyntax::RefCast<Declaration>(syntax));
            break;
        case SyntaxType::CompoundBlock:
            GenerateCompoundBlock(AbstractSyntax::RefCast<CompoundBlock>(syntax));
            break;
        case SyntaxType::IfStatement:
            GenerateIfStatement(AbstractSyntax::RefCast<IfStatement>(syntax));
            break; 
        case SyntaxType::Return:
            GenerateReturn(AbstractSyntax::RefCast<Return>(syntax)); 
            break; 
        case SyntaxType::IntConstant:
            GenerateIntConstant(AbstractSyntax::RefCast<IntConstant>(syntax)); 
            break;
        case SyntaxType::UnaryOp:
            GenerateUnaryOp(AbstractSyntax::RefCast<UnaryOp>(syntax)); 
            break;
        case SyntaxType::BinaryOp:
            GenerateBinaryOp(AbstractSyntax::RefCast<BinaryOp>(syntax)); 
            break; 
        case SyntaxType::VariableRef:
            GenerateVariableRef(AbstractSyntax::RefCast<VariableRef>(syntax));
            break;
        case SyntaxType::Assignment:
            GenerateAssignment(AbstractSyntax::RefCast<Assignment>(syntax));
            break;
    }
}

void ASMGenerator::GenerateProgram(Program::Ref program)
{
    GenerateFunction(program->function); 
}

void ASMGenerator::GenerateFunction(Function::Ref function)
{
    m_ContextStack.push(CreateRef<BlockContext>()); 
    auto& context = m_ContextStack.top(); 
    m_CodeGenerator.EmitFun(function->name); 
    m_CodeGenerator.IncreaseIndentation(); 
    // generate function prologue
    m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RBP));
    m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::RSP), RegisterArg(Register::RBP));
    // calculate stack size
    auto stack_size = AccumVarSize(function->block);
    // align size to nearest higher multiple of 16
    stack_size = stack_size == 0 ? 0 : ((stack_size - 1) | 15) + 1;
    if (stack_size > 0)
        m_CodeGenerator.EmitOp(OpInstruction::SUB, ImmediateArg(stack_size), RegisterArg(Register::RSP));
    // insert return statement if one does not exist
    bool foundReturn = false; 
    for (auto statement : function->Statements())
    {
        if (statement->type() == SyntaxType::Return)
        {
            foundReturn = true; 
            break; 
        }
    }
    if (!foundReturn) function->block->statements.emplace_back(CreateRef<Return>(CreateRef<IntConstant>(0))); 
    // generate function body
    GenerateCompoundBlock(function->block); 
    m_CodeGenerator.DecreaseIndentation();
}

void ASMGenerator::GenerateStatementExpression(StatementExpression::Ref statementExpression)
{
    if (statementExpression->expr) GenerateSyntax(statementExpression->expr); 
}

void ASMGenerator::GenerateAssignmentOp(AssignmentOp::Ref op)
{
    auto& context = m_ContextStack.top(); 
    if (!context->HasVariable(op->lvalue))
        throw UndeclaredException(op->lvalue);
    auto dst = PointerArg(Register::RBP, context->GetVariableOffset(op->lvalue));
    auto eax = RegisterArg(Register::EAX);
    auto src = LoadRegister(op->rvalue, Register::ECX);
    switch (op->OpType())
    {
        case AssignmentOpType::Add:
            m_CodeGenerator.EmitOp(OpInstruction::ADD, src, dst); 
            break;
        case AssignmentOpType::Minus:
            m_CodeGenerator.EmitOp(OpInstruction::SUB, src, dst); 
            break; 
        case AssignmentOpType::Multiplication:
            m_CodeGenerator.EmitOp(OpInstruction::IMUL, src, dst); 
            break;
        case AssignmentOpType::Division:
            m_CodeGenerator.EmitOp(OpInstruction::MOV, dst, eax); 
            m_CodeGenerator.EmitOp(OpInstruction::CDQ); 
            m_CodeGenerator.EmitOp(OpInstruction::IDIV, src);
            break; 
        case AssignmentOpType::Modulo:
            m_CodeGenerator.EmitOp(OpInstruction::MOV, dst, eax); 
            m_CodeGenerator.EmitOp(OpInstruction::CDQ);
            m_CodeGenerator.EmitOp(OpInstruction::IDIV, src); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::EDX), dst); 
            break;
        case AssignmentOpType::LeftShift:
            m_CodeGenerator.EmitOp(OpInstruction::SAL, src, dst);
            break;
        case AssignmentOpType::RightShift:
            m_CodeGenerator.EmitOp(OpInstruction::SAR, src, dst); 
            break;
        case AssignmentOpType::LogicalOr:
            m_CodeGenerator.EmitOp(OpInstruction::OR, src, dst); 
            break;
        case AssignmentOpType::LogicalAnd:
            m_CodeGenerator.EmitOp(OpInstruction::AND, src, dst); 
            break;
        case AssignmentOpType::LogicalXOR:
            m_CodeGenerator.EmitOp(OpInstruction::XOR, src, dst); 
            break; 
    }
    m_CodeGenerator.EmitOp(OpInstruction::MOV, dst, ActiveRegister());
}

void ASMGenerator::GenerateDeclaration(Declaration::Ref decl)
{
    auto& context = m_ContextStack.top(); 
    for (auto var : decl->variables)
    {
        if (context->HasScopeVariable(var.name))
            throw RedeclaredException(var.name);
        auto offset = context->CreateVariable(var.name, 4); 
        PushRegister(Register::EAX);
        if (var.expr != nullptr) GenerateSyntax(var.expr); 
        else m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, ImmediateArg(0), ActiveRegister()); 
        m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::EAX), PointerArg(Register::RBP, offset));
        PopRegister(); 
    }
}

void ASMGenerator::GenerateCompoundBlock(CompoundBlock::Ref compoundBlock)
{
    m_ContextStack.push(CreateRef<BlockContext>(m_ContextStack.top()));
    for (auto statement : compoundBlock->statements)
    {
        GenerateSyntax(statement);
        // anything after a return is dead code
        if (statement->type() == SyntaxType::Return) break;
    }
    m_ContextStack.pop(); 
}

void ASMGenerator::GenerateIfStatement(IfStatement::Ref ifStatement)
{
    bool has_else = ifStatement->else_statement != nullptr; 
    auto end = m_CodeGenerator.GenerateLabel();
    std::vector<LabelArg> labels;
    for (auto else_if : ifStatement->else_ifs)
        labels.emplace_back(m_CodeGenerator.GenerateLabel());
    if (has_else)
        labels.emplace_back(m_CodeGenerator.GenerateLabel()); 
    LoadRegister(ifStatement->if_conditional.condition, Register::EAX);
    m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), RegisterArg(Register::EAX)); 
    m_CodeGenerator.EmitOp(OpInstruction::JE, labels.empty() ? end : labels[0]);
    GenerateSyntax(ifStatement->if_conditional.statement);
    if (!labels.empty())
        m_CodeGenerator.EmitOp(OpInstruction::JMP, end);
    size_t len = ifStatement->else_ifs.size();
    for (size_t i = 0; i < len; i++)
    {
        auto& else_if = ifStatement->else_ifs[i];
        m_CodeGenerator.EmitLabel(labels[i]);
        LoadRegister(else_if.condition, Register::EAX);
        bool last = labels.size() == len && i == len - 1;
        if (!last)
        {
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), RegisterArg(Register::EAX)); 
            m_CodeGenerator.EmitOp(OpInstruction::JE, i < labels.size() - 1 ? labels[i + 1] : end);
        }
        GenerateSyntax(else_if.statement); 
        if (!last) m_CodeGenerator.EmitOp(OpInstruction::JMP, end);
    }
    if (has_else)    
    {
        m_CodeGenerator.EmitLabel(labels[labels.size() - 1]);       
        GenerateSyntax(ifStatement->else_statement);
    }
    m_CodeGenerator.EmitLabel(end); 
}

void ASMGenerator::GenerateReturn(Return::Ref ret)
{
    if (ret->expr) LoadRegister(ret->expr, Register::EAX);
    // generate function epilogue
    m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::RBP), RegisterArg(Register::RSP)); 
    m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RBP));
    // return
    m_CodeGenerator.EmitOp(OpInstruction::RET);
}

void ASMGenerator::GenerateIntConstant(IntConstant::Ref expr)
{
    m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, ImmediateArg(expr->value), ActiveRegister()); 
}

void ASMGenerator::GenerateUnaryOp(UnaryOp::Ref op)
{
    switch (op->OpType())
    {
        case UnaryOpType::Negation:
            GenerateSyntax(op->expr); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::NEG, ActiveRegister()); 
            break;
        case UnaryOpType::Complement:
            GenerateSyntax(op->expr); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::NOT, ActiveRegister());
            break;
        case UnaryOpType::LogicalNegation:
            PushRegister(Register::ECX);
            GenerateSyntax(op->expr);
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), ActiveRegister());
            m_CodeGenerator.EmitOp(OpInstruction::SETE, RegisterArg(Register::AL)); 
            if (!ActiveRegister().is_eax())
                m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::EAX), ActiveRegister());
            PopRegister();
            break;
    }
}

void ASMGenerator::GenerateBinaryOp(BinaryOp::Ref op)
{
    switch (op->OpType())
    { 
        case BinaryOpType::Addition:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::ADD, src, dst); 
            break;
        }
        case BinaryOpType::Subtraction:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::SUB, src, dst);
            break;
        }
        case BinaryOpType::Multiplication:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::IMUL, src, dst);
            break;
        }
        case BinaryOpType::Division:
        {
            LoadRegister(op->lvalue, Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RAX));
            auto divisor = LoadRegister(op->rvalue, Register::ECX);
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RAX));
            m_CodeGenerator.EmitOp(OpInstruction::CDQ);
            m_CodeGenerator.EmitOp(OpInstruction::IDIV, divisor); 
            break; 
        }
        case BinaryOpType::LogicalOr: 
        {
            auto end = m_CodeGenerator.GenerateLabel(); 
            auto clause2 = m_CodeGenerator.GenerateLabel();
            auto lvalue = LoadRegister(op->lvalue, Register::EDX);
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), lvalue);
            m_CodeGenerator.EmitOp(OpInstruction::JE, clause2); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, ImmediateArg(1), RegisterArg(Register::EAX));
            m_CodeGenerator.EmitOp(OpInstruction::JMP, end);
            m_CodeGenerator.EmitLabel(clause2);
            auto rvalue = LoadRegister(op->rvalue, Register::EDX); 
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), rvalue); 
            m_CodeGenerator.EmitOp(OpInstruction::SETNE, RegisterArg(Register::AL)); 
            m_CodeGenerator.EmitLabel(end);
            break;
        }
        case BinaryOpType::LogicalAnd: 
        {
            auto end = m_CodeGenerator.GenerateLabel(); 
            auto clause2 = m_CodeGenerator.GenerateLabel();
            auto lvalue = LoadRegister(op->lvalue, Register::EDX);
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), lvalue);
            m_CodeGenerator.EmitOp(OpInstruction::JNE, clause2); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, ImmediateArg(1), RegisterArg(Register::EAX));
            m_CodeGenerator.EmitOp(OpInstruction::JMP, end);
            m_CodeGenerator.EmitLabel(clause2);
            auto rvalue = LoadRegister(op->rvalue, Register::EDX); 
            m_CodeGenerator.EmitOp(OpInstruction::CMP, ImmediateArg(0), rvalue); 
            m_CodeGenerator.EmitOp(OpInstruction::SETNE, RegisterArg(Register::AL)); 
            m_CodeGenerator.EmitLabel(end);
            break;
        }
        case BinaryOpType::Equal:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETE, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::NotEqual:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETNE, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::LessThan:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
             m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETL, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::LessThanOrEqual:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
             m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETLE, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::GreaterThan:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
             m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETG, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::GreaterThanOrEqual:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::ECX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            ZeroRegister(Register::EAX);
            m_CodeGenerator.EmitOp(OpInstruction::CMP, src, dst);
            m_CodeGenerator.EmitOp(OpInstruction::SETGE, RegisterArg(Register::AL)); 
            break;
        }
        case BinaryOpType::Remainder:
        {
            LoadRegister(op->lvalue, Register::EAX);
            PushRegister(Register::RAX);
            auto divisor = LoadRegister(op->rvalue, Register::ECX);
            PopRegister(); 
            m_CodeGenerator.EmitOp(OpInstruction::CDQ);
            m_CodeGenerator.EmitOp(OpInstruction::IDIV, divisor); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::EDX), RegisterArg(Register::EAX)); 
            break; 
        }
        case BinaryOpType::BitwiseOr:
        {
            auto src = LoadRegister(op->lvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::OR, src, dst); 
            break;
        }
        case BinaryOpType::BitwiseAnd:
        {
            auto src = LoadRegister(op->lvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::AND, src, dst); 
            break;
        }
        case BinaryOpType::BitwiseXOR:
        {
            auto src = LoadRegister(op->lvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::XOR, src, dst); 
            break;
        }
        case BinaryOpType::BitwiseLeftShift:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::SAL, src, dst);
            break;
        }
        case BinaryOpType::BitwiseRightShift:
        {
            auto src = LoadRegister(op->rvalue, Register::EDX);
            m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            auto dst = LoadRegister(op->lvalue, Register::EAX); 
            m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX));
            m_CodeGenerator.EmitOp(OpInstruction::SAR, src, dst);
            break;
        }
        case BinaryOpType::Comma:
        {
            // evaluate both operands
            LoadRegister(op->lvalue, Register::EAX);
            LoadRegister(op->rvalue, Register::EAX);
            break;
        }
    }
    if (!ActiveRegister().is_eax())
        m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::EAX), ActiveRegister()); 
}

void ASMGenerator::GenerateVariableRef(VariableRef::Ref ref)
{
    auto& context = m_ContextStack.top();
    if (!context->HasVariable(ref->name))
        throw UndeclaredException(ref->name); 
    auto offset = context->GetVariableOffset(ref->name); 
    m_CodeGenerator.EmitOp(OpInstruction::MOV, PointerArg(Register::RBP, offset), ActiveRegister()); 
}

void ASMGenerator::GenerateAssignment(Assignment::Ref assignment)
{
    auto& context = m_ContextStack.top(); 
    if (!context->HasVariable(assignment->lvalue))
        throw UndeclaredException(assignment->lvalue);
    GenerateSyntax(assignment->rvalue);
    auto offset = context->GetVariableOffset(assignment->lvalue);
    m_CodeGenerator.EmitOp(OpInstruction::MOV, ActiveRegister(), PointerArg(Register::RBP, offset)); 
}