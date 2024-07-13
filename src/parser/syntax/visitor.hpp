#pragma once

class Program;
class Function; 
class Return; 
class IntExpr; 

class ASTVisitor
{

public:
    virtual void visitProgram(Program* program) = 0;
    virtual void visitFunction(Function* function) = 0;
    virtual void visitReturn(Return* ret) = 0;
    virtual void visitIntExpr(IntExpr* expr) = 0;

};