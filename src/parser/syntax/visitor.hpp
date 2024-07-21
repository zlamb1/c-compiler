#pragma once

class Program;
class Function; 
class Return; 
class IntConstant; 
class UnaryOp; 
class BinaryOp; 

class ASTVisitor
{

public:
    virtual void VisitProgram(Program* program) = 0;
    virtual void VisitFunction(Function* function) = 0;
    virtual void VisitReturn(Return* ret) = 0;
    virtual void VisitIntConstant(IntConstant* expr) = 0;
    virtual void VisitUnaryOp(UnaryOp* op) = 0; 
    virtual void VisitBinaryOp(BinaryOp* op) = 0; 

};