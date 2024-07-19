#pragma once

class Program;
class Function; 
class Return; 
class IntExpr; 
class UnaryOp; 

class ASTVisitor
{

public:
    virtual void VisitProgram(Program* program) = 0;
    virtual void VisitFunction(Function* function) = 0;
    virtual void VisitReturn(Return* ret) = 0;
    virtual void VisitIntExpr(IntExpr* expr) = 0;
    virtual void VisitUnaryOp(UnaryOp* op) = 0; 

};