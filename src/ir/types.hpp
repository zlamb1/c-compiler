#pragma once

#include <unordered_map>
#include <variant>

#include "parser/syntax/includes.hpp"

namespace TAC
{
    enum class OpCode : int
    {
        NEG,
        NOT,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        AND,
        OR,
        XOR,
        LSH,
        RSH,
        EQL,
        NEQL,
        LT,
        LTE,
        GT,
        GTE,
        BOOL_NOT,
        BOOL_AND,
        BOOL_OR,
        ERR
    };
    static std::unordered_map<OpCode, std::string> OP_CODE_SYMBOLS 
    {
        { OpCode::NEG,      "-"   },
        { OpCode::NOT,      "~"   },
        { OpCode::ADD,      "+"   },
        { OpCode::SUB,      "-"   },
        { OpCode::MUL,      "*"   },
        { OpCode::DIV,      "/"   },
        { OpCode::MOD,      "%"   },
        { OpCode::AND,      "&"   },
        { OpCode::OR,       "|"   },
        { OpCode::XOR,      "^"   },
        { OpCode::LSH,      "<<"  },
        { OpCode::RSH,      ">>"  },
        { OpCode::EQL,      "=="  },
        { OpCode::NEQL,     "!="  },
        { OpCode::LT,       "<"   },
        { OpCode::LTE,      "<="  },
        { OpCode::GT,       ">"   },
        { OpCode::GTE,      ">="  },
        { OpCode::BOOL_NOT, "!"   },
        { OpCode::BOOL_AND, "&&"  },
        { OpCode::BOOL_OR,  "||"  },
        { OpCode::ERR,      "ERR" }
    };
    enum class OperandType : int
    {
        Constant,
        Ref
    };
    class Operand
    {
    public:
        std::variant<IntConstant::Ref, VariableRef::Ref> member; 
        Operand(IntConstant::Ref constant) : m_Type(OperandType::Constant), member(constant)
        {
        }
        Operand(VariableRef::Ref ref) : m_Type(OperandType::Ref), member(ref)
        {
        }
        std::string to_string()
        {
            return m_Type == OperandType::Constant ? 
                std::to_string(std::get<IntConstant::Ref>(member)->value) :
                std::get<VariableRef::Ref>(member)->name;
        }
        int get_value() { return std::get<IntConstant::Ref>(member)->value; }
        std::string get_name() { return std::get<VariableRef::Ref>(member)->name; }
        OperandType type() { return m_Type; }
        typedef std::shared_ptr<Operand> Ref; 
    private:
        OperandType m_Type; 
    };
    enum class StatementType : int
    {
        Goto,
        Condition,
        Assign,
        Triple,
        Quad,
        Label,
        Return
    };
    class Statement 
    {
    public:
        virtual ~Statement() = default;
        StatementType type() { return m_Type; }
        typedef std::shared_ptr<Statement> Ref; 
        template<typename T>
        static std::shared_ptr<T> RefCast(Ref ref)
        {
            return std::dynamic_pointer_cast<T>(ref); 
        }
    protected:
        StatementType m_Type;
        Statement(StatementType type) : m_Type(type)
        {
        }
    };
    struct GotoStatement : public Statement
    {
        std::string label;

        GotoStatement(const std::string& label) : Statement(StatementType::Goto), label(label)
        {
        }

        typedef std::shared_ptr<GotoStatement> Ref; 
    };
    struct ConditionStatement : public Statement
    {
        Operand::Ref condition; 
        std::string goto_label; 

        ConditionStatement(Operand::Ref condition, const std::string& goto_label) : Statement(StatementType::Condition), condition(condition), goto_label(goto_label)
        {
        }

        typedef std::shared_ptr<ConditionStatement> Ref; 
    };
    struct AssignStatement : public Statement
    {
        VariableRef::Ref lhs; 
        Operand::Ref rhs;

        AssignStatement(VariableRef::Ref lhs, Operand::Ref rhs) : Statement(StatementType::Assign), lhs(lhs), rhs(rhs)
        {
        } 

        typedef std::shared_ptr<AssignStatement> Ref; 
    };
    struct TripleStatement : public Statement
    {
        OpCode op; 
        Operand::Ref rhs; 
        VariableRef::Ref result; 

        TripleStatement(OpCode op, Operand::Ref rhs, VariableRef::Ref result) : Statement(StatementType::Triple), op(op), rhs(rhs), result(result)
        {
        }

        typedef std::shared_ptr<TripleStatement> Ref; 
    };
    struct QuadStatement : public Statement
    {
        OpCode op;
        Operand::Ref lhs, rhs; 
        VariableRef::Ref result; 

        QuadStatement(OpCode op, Operand::Ref lhs, Operand::Ref rhs, VariableRef::Ref result) : Statement(StatementType::Quad), op(op), lhs(lhs), rhs(rhs), result(result) 
        {
        }

        typedef std::shared_ptr<QuadStatement> Ref;         
    };
    struct LabelStatement : public Statement
    {
        std::string name; 

        LabelStatement(const std::string& name) : Statement(StatementType::Label), name(name)
        {
        }

        typedef std::shared_ptr<LabelStatement> Ref; 
    };
    struct ReturnStatement : public Statement
    {
        Operand::Ref ret_val; 

        ReturnStatement(Operand::Ref ret_val) : Statement(StatementType::Return), ret_val(ret_val)
        {
        }

        typedef std::shared_ptr<ReturnStatement> Ref; 
    };
};