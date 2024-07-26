#pragma once

#include "types.hpp"

namespace TAC
{
    static inline OpCode convert_unary_op(UnaryOpType type)
    {
        switch (type)
        {
            case UnaryOpType::Negation:
                return OpCode::NEG;
            case UnaryOpType::Complement:
                return OpCode::NOT;
            case UnaryOpType::LogicalNegation:
                return OpCode::BOOL_NOT;
        }
        return OpCode::ERR; 
    }
    static inline OpCode convert_binary_op(BinaryOpType type)
    {
        switch (type)
        {
            case BinaryOpType::Addition:
                return OpCode::ADD;
            case BinaryOpType::Subtraction:
                return OpCode::SUB;
            case BinaryOpType::Multiplication:
                return OpCode::MUL;
            case BinaryOpType::Division:
                return OpCode::DIV;
            case BinaryOpType::LogicalOr:
                return OpCode::BOOL_OR;
            case BinaryOpType::LogicalAnd:
                return OpCode::BOOL_AND;
            case BinaryOpType::Equal:
                return OpCode::EQL;
            case BinaryOpType::NotEqual:
                return OpCode::NEQL;
            case BinaryOpType::LessThan:
                return OpCode::LT;
            case BinaryOpType::LessThanOrEqual:
                return OpCode::LTE;
            case BinaryOpType::GreaterThan:
                return OpCode::GT;
            case BinaryOpType::GreaterThanOrEqual:
                return OpCode::GTE;
            case BinaryOpType::Remainder:
                return OpCode::MOD;
            case BinaryOpType::BitwiseOr:
                return OpCode::OR;
            case BinaryOpType::BitwiseAnd:
                return OpCode::AND;
            case BinaryOpType::BitwiseXOR:
                return OpCode::XOR;
            case BinaryOpType::BitwiseLeftShift:
                return OpCode::LSH;
            case BinaryOpType::BitwiseRightShift:
                return OpCode::RSH;
        }
        return OpCode::ERR;
    }
    static inline OpCode convert_assignment_op(AssignmentOpType type)
    {
        switch (type)
        {
            case AssignmentOpType::Add:
                return OpCode::ADD;
            case AssignmentOpType::Minus:
                return OpCode::SUB;
            case AssignmentOpType::Multiplication:
                return OpCode::MUL;
            case AssignmentOpType::Division:
                return OpCode::DIV;
            case AssignmentOpType::Modulo:
                return OpCode::MOD;
            case AssignmentOpType::LeftShift:
                return OpCode::LSH;
            case AssignmentOpType::RightShift:
                return OpCode::RSH;
            case AssignmentOpType::LogicalOr:
                return OpCode::BOOL_OR;
            case AssignmentOpType::LogicalAnd:
                return OpCode::BOOL_AND;
            case AssignmentOpType::LogicalXOR:
                return OpCode::XOR;
        }
        return OpCode::ERR;
    }
};