#include "generator.hpp"

void ASMGenerator::GenerateAssembly(TACGenerator& generator)
{
    auto statements = generator.GetStatements();
    auto var_context = generator.GetVarContext(); 
    m_CodeGenerator.EmitFun("main"); 
    m_CodeGenerator.IncreaseIndentation(); 
    // calculate stack size
    for (auto statement : statements)
    {
        if (statement->type() == TAC::StatementType::Assign)
        {
            auto assign = TAC::Statement::RefCast<TAC::AssignStatement>(statement);
            auto symbol = assign->lhs; 
            if (!symbol->is_temp && m_LocationTable.find(symbol) == m_LocationTable.end())
            {
                m_StackIndex -= symbol->byte_size;
                auto loc = VarLocation(m_StackIndex); 
                m_LocationTable[symbol] = loc;
            }
        }
    }
    // align stack size to multiple of 16
    auto stack_size = std::abs(m_StackIndex); 
    stack_size = stack_size == 0 ? 0 : ((stack_size - 1) | 15) + 1;
    // generate function prologue
    m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RBP));
    m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::RSP), RegisterArg(Register::RBP)); 
    if (stack_size > 0) m_CodeGenerator.EmitOp(OpInstruction::SUB, ImmediateArg(stack_size), RegisterArg(Register::RSP)); 
    TAC::ReturnStatement::Ref returnStatement = nullptr; 
    for (size_t i = 0; i < statements.size(); i++)
    {
        auto statement = statements[i]; 
        switch (statement->type())
        {
            case TAC::StatementType::Goto:
            {
                auto _goto = TAC::Statement::RefCast<TAC::GotoStatement>(statement); 
                m_CodeGenerator.EmitOp(OpInstruction::JMP, LabelArg(_goto->label));
                break;
            }
            case TAC::StatementType::Condition:
            {
                auto cond_statement = TAC::Statement::RefCast<TAC::ConditionStatement>(statement);
                auto loc = FetchVarLocation(cond_statement->condition); 
                if (loc->type() == ArgType::Immediate)
                {
                    auto _reg = allocator.AllocRegister(); 
                    m_CodeGenerator.EmitOp(OpInstruction::MOV, loc, RegisterArg(_reg)); 
                    loc = CreateRef<RegisterArg>(_reg); 
                    allocator.FreeRegister(_reg); 
                }
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::CMP, ImmediateArg(0), *loc.get()); 
                m_CodeGenerator.EmitOp(OpInstruction::JNE, LabelArg(cond_statement->goto_label)); 
                break;
            }
            case TAC::StatementType::Assign:
            {
                auto assign = TAC::Statement::RefCast<TAC::AssignStatement>(statement);
                auto src = FetchVarLocation(assign->rhs); 
                auto dst = FetchVarLocation(var_context, assign->lhs); 
                if (IsPointer(src) && IsPointer(dst))
                {
                    auto _reg = allocator.AllocRegister();
                    m_CodeGenerator.EmitOp(OpInstruction::MOV, src, RegisterArg(_reg));
                    src = CreateRef<RegisterArg>(_reg); 
                    allocator.FreeRegister(_reg);
                }
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *src.get(), *dst.get());
                break;
            }
            case TAC::StatementType::Triple:
            {
                auto triple = TAC::Statement::RefCast<TAC::TripleStatement>(statement);
                GenerateTriple(var_context, triple); 
                break;
            }
            case TAC::StatementType::Quad:
            {
                auto quad = TAC::Statement::RefCast<TAC::QuadStatement>(statement);
                GenerateQuad(var_context, quad);
                break;
            }
            case TAC::StatementType::Label:
            {
                auto label = TAC::Statement::RefCast<TAC::LabelStatement>(statement); 
                m_CodeGenerator.EmitLabel(LabelArg(label->name));
                break;
            }
            case TAC::StatementType::Return:
                returnStatement = TAC::Statement::RefCast<TAC::ReturnStatement>(statement); 
                goto end; 
        }
        // free temp registers
        for (auto pair : m_LocationTable)
        {
            auto symbol = pair.first;
            if (symbol->is_temp && symbol->range.end == i)
                allocator.FreeRegister(m_LocationTable[symbol]._register);
        }
    }
    end:
    // move ret val into EAX
    if (returnStatement != nullptr)
    {  
        auto ret_val = returnStatement->ret_val;
        auto src = FetchVarLocation(ret_val); 
        if (!IsRegister(src, Register::EAX))
            m_CodeGenerator.EmitOp(OpInstruction::MOV, src, RegisterArg(Register::EAX)); 
    }
    else m_CodeGenerator.EmitOp(OpInstruction::MOV, ImmediateArg(0), RegisterArg(Register::EAX)); 
    // generate function prologue
    m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(Register::RBP), RegisterArg(Register::RSP)); 
    m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RBP));
    m_CodeGenerator.EmitOp(OpInstruction::RET); 
    m_CodeGenerator.DecreaseIndentation();
}

void ASMGenerator::GenerateTriple(VarContext& var_context, TAC::TripleStatement::Ref triple)
{
    auto op = triple->op; 
    AllocIfTemp(var_context, triple->dst);
    
    auto src = FetchVarLocation(triple->rhs); 
    auto dst_loc = FetchVarLocation(var_context, triple->dst); 
    switch (op)
    {
        case TAC::OpCode::NEG:
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *src.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::NEG, *dst_loc.get()); 
            break;
        case TAC::OpCode::NOT:
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *src.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::NOT, *dst_loc.get()); 
            break;
        case TAC::OpCode::BOOL_NOT:
        {
            auto _reg = allocator.AllocRegister(); 
            auto lower_byte = RegisterUtility::get_lower_byte(_reg);
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *src.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::CMP, ImmediateArg(0), *dst_loc.get());
            m_CodeGenerator.EmitOp(OpInstruction::SETE, RegisterArg(lower_byte));
            // sign-extend the lower byte
            m_CodeGenerator.EmitOp(OpInstruction::MOVZ, RegisterArg(lower_byte), RegisterArg(_reg)); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(_reg), dst_loc);
            allocator.FreeRegister(_reg); 
            break;
        }
    }
}

void ASMGenerator::GenerateQuad(VarContext& var_context, TAC::QuadStatement::Ref quad)
{
    auto op = quad->op; 
    AllocIfTemp(var_context, quad->dst);
    auto lhs_loc = FetchVarLocation(quad->lhs);
    auto rhs_loc = FetchVarLocation(quad->rhs); 
    auto dst_loc = FetchVarLocation(var_context, quad->dst); 
    switch (op)
    {
        case TAC::OpCode::ADD:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::ADD, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::SUB:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::SUB, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::MUL:
            // dst operand of IMUL cannot be memory pointer
            if (dst_loc->type() == ArgType::Pointer)
            {
                auto _reg = allocator.AllocRegister(); 
                m_CodeGenerator.EmitOp(OpInstruction::MOV, *lhs_loc.get(), RegisterArg(_reg)); 
                m_CodeGenerator.EmitOp(OpInstruction::IMUL, *rhs_loc.get(), RegisterArg(_reg)); 
                m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(_reg), *dst_loc.get());
                allocator.FreeRegister(_reg);
                break;
            }
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::IMUL, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::DIV:
        case TAC::OpCode::MOD:
        {
            // spill EAX/EDX if necessary
            bool spilledEAX = false, spilledEDX = false;
            if (!allocator.IsRegisterFree(Register::EAX))
            {
                if (!IsRegister(dst_loc, Register::EAX) && !IsRegister(lhs_loc, Register::EAX) && !IsRegister(rhs_loc, Register::EAX))
                {
                    spilledEAX = true;
                    m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RAX));   
                }
            } else allocator.AllocRegister(Register::EAX); 
            if (!allocator.IsRegisterFree(Register::EDX))
            {
                spilledEDX = true; 
                m_CodeGenerator.EmitOp(OpInstruction::PUSH, RegisterArg(Register::RDX));
            } else allocator.AllocRegister(Register::EDX);
            if (!IsRegister(lhs_loc, Register::EAX)) 
                m_CodeGenerator.EmitOp(OpInstruction::MOV, lhs_loc, RegisterArg(Register::EAX)); 
            m_CodeGenerator.EmitOp(OpInstruction::CDQ); 
            if (rhs_loc->type() == ArgType::Immediate)
            {
                auto _reg = allocator.AllocRegister(); 
                m_CodeGenerator.EmitOp(OpInstruction::MOV, rhs_loc, RegisterArg(_reg));
                rhs_loc = CreateRef<RegisterArg>(_reg);
                allocator.FreeRegister(_reg);
            }
            m_CodeGenerator.EmitOp(OpInstruction::IDIV, rhs_loc); 
            auto dst_reg = op == TAC::OpCode::DIV ? Register::EAX : Register::EDX; 
            if (!IsRegister(dst_loc, Register::EAX)) 
                m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(dst_reg), dst_loc);
            if (spilledEAX) m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RAX));
            else allocator.FreeRegister(Register::EAX);
            if (spilledEDX) m_CodeGenerator.EmitOp(OpInstruction::POP, RegisterArg(Register::RDX)); 
            else allocator.FreeRegister(Register::EDX); 
            break;
        }
        case TAC::OpCode::AND:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::AND, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::OR:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::OR, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::XOR:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::XOR, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::LSH:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::SAL, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::RSH:
            if (!IsSameRegister(lhs_loc, dst_loc))
                m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::MOV, *lhs_loc.get(), *dst_loc.get()); 
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::SAR, *rhs_loc.get(), *dst_loc.get()); 
            break;
        case TAC::OpCode::EQL:
        case TAC::OpCode::NEQL:
        case TAC::OpCode::LT:
        case TAC::OpCode::LTE:
        case TAC::OpCode::GT:
        case TAC::OpCode::GTE:
        {
            auto _reg = allocator.AllocRegister(); 
            auto lower_byte = RegisterUtility::get_lower_byte(_reg);
            // swap sides to get proper comparisons
            std::swap(lhs_loc, rhs_loc);
            // load rhs if immediate
            if (IsImmediate(rhs_loc))
            {
                auto _reg = allocator.AllocRegister();
                m_CodeGenerator.EmitOp(OpInstruction::MOV, rhs_loc, RegisterArg(_reg)); 
                rhs_loc = CreateRef<RegisterArg>(_reg); 
                allocator.FreeRegister(_reg);
            }
            m_CodeGenerator.EmitOp<OperandSize::DWORD>(OpInstruction::CMP, *lhs_loc.get(), *rhs_loc.get());
            auto comp = OpInstruction::SETE;
            switch (op)
            {
                case TAC::OpCode::NEQL: comp = OpInstruction::SETNE; break;
                case TAC::OpCode::LT:   comp = OpInstruction::SETL;  break;
                case TAC::OpCode::LTE:  comp = OpInstruction::SETLE; break;
                case TAC::OpCode::GT:   comp = OpInstruction::SETG;  break;
                case TAC::OpCode::GTE:  comp = OpInstruction::SETGE; break;
            }
            m_CodeGenerator.EmitOp(comp, RegisterArg(lower_byte));
            // sign-extend lower byte
            m_CodeGenerator.EmitOp(OpInstruction::MOVZ, RegisterArg(lower_byte), RegisterArg(_reg)); 
            m_CodeGenerator.EmitOp(OpInstruction::MOV, RegisterArg(_reg), dst_loc);
            allocator.FreeRegister(_reg); 
            break;
        }
    } 
}