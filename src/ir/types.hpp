#pragma once

#include <unordered_map>
#include <variant>

#include "parser/syntax/includes.hpp"
#include "symbol.hpp"

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
        Symbol,
        Label
    };
    class Operand
    {
    public:
        std::variant<IntConstant::Ref, VarSymbol::Ref, std::string> member; 
        
        Operand(IntConstant::Ref constant) : m_Type(OperandType::Constant), member(constant)
        {
        }

        Operand(VarSymbol::Ref var_symbol) : m_Type(OperandType::Symbol), member(var_symbol)
        {
        }

        Operand(const std::string& label) : m_Type(OperandType::Label), member(label)
        {
        }

        std::string to_string()
        {
            switch (m_Type)
            {
                case OperandType::Constant:
                    return std::to_string(std::get<IntConstant::Ref>(member)->value);
                case OperandType::Symbol:
                    return std::get<VarSymbol::Ref>(member)->var_name;
                case OperandType::Label:
                    return std::get<std::string>(member);
            }
        }

        int get_value() { return std::get<IntConstant::Ref>(member)->value; }
        VarSymbol::Ref get_symbol() { return std::get<VarSymbol::Ref>(member); }
        const std::string& get_label() { return std::get<std::string>(member); }
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
        int value = 0;

        ConditionStatement(Operand::Ref condition, const std::string& goto_label) : Statement(StatementType::Condition), condition(condition), goto_label(goto_label)
        {
        }

        ConditionStatement(Operand::Ref condition, const std::string& goto_label, int value) : 
            Statement(StatementType::Condition), condition(condition), goto_label(goto_label), value(value)
        {
        }

        typedef std::shared_ptr<ConditionStatement> Ref; 
    };
    struct AssignStatement : public Statement
    {
        VarSymbol::Ref lhs; 
        Operand::Ref rhs;

        AssignStatement(VarSymbol::Ref lhs, Operand::Ref rhs) : Statement(StatementType::Assign), lhs(lhs), rhs(rhs)
        {
        } 

        typedef std::shared_ptr<AssignStatement> Ref; 
    };
    struct TripleStatement : public Statement
    {
        OpCode op; 
        Operand::Ref rhs; 
        VarSymbol::Ref dst; 

        TripleStatement(OpCode op, Operand::Ref rhs, VarSymbol::Ref dst) : Statement(StatementType::Triple), op(op), rhs(rhs), dst(dst)
        {
        }

        typedef std::shared_ptr<TripleStatement> Ref; 
    };
    struct QuadStatement : public Statement
    {
        OpCode op;
        Operand::Ref lhs, rhs; 
        VarSymbol::Ref dst; 

        QuadStatement(OpCode op, Operand::Ref lhs, Operand::Ref rhs, VarSymbol::Ref dst) : Statement(StatementType::Quad), op(op), lhs(lhs), rhs(rhs), dst(dst) 
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
    struct Function 
    {
        std::string function_name;
        std::vector<Statement::Ref> statements{};
        size_t label_counter = 0, temp_counter = 0; 

        Function(const std::string& function_name) : function_name(function_name)
        {
        }

        typedef std::shared_ptr<Function> Ref; 
    };
};