#pragma once

struct Program;
struct Function; 
struct StatementExpression;
struct Declaration; 
struct Return; 
struct IntConstant; 
struct UnaryOp; 
struct BinaryOp; 
struct VariableRef; 
struct Assignment;

class ASTVisitor
{
public:
    virtual void VisitProgram(Program* program) = 0;
    virtual void VisitFunction(Function* function) = 0;
    virtual void VisitStatementExpression(StatementExpression* statementExpr) = 0;
    virtual void VisitDeclaration(Declaration* decl) = 0; 
    virtual void VisitReturn(Return* ret) = 0;
    virtual void VisitIntConstant(IntConstant* expr) = 0;
    virtual void VisitUnaryOp(UnaryOp* op) = 0; 
    virtual void VisitBinaryOp(BinaryOp* op) = 0; 
    virtual void VisitVariableRef(VariableRef* ref) = 0; 
    virtual void VisitAssignment(Assignment* assignment) = 0; 
};