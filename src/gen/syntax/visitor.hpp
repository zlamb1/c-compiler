#pragma once

struct RegisterArg;
struct ImmediateArg; 
struct LabelArg; 

class SyntaxVisitor
{
    public:
        virtual void VisitRegisterArg(RegisterArg* arg) = 0;
        virtual void VisitImmediateArg(ImmediateArg* arg) = 0; 
        virtual void VisitLabelArg(LabelArg* arg) = 0; 
};