#pragma once

struct RegisterArg;
struct OffsetArg; 
struct ImmediateArg; 
struct LabelArg; 

class SyntaxVisitor
{
    public:
        virtual void VisitRegisterArg(RegisterArg* arg) = 0;
        virtual void VisitOffsetArg(OffsetArg* arg) = 0; 
        virtual void VisitImmediateArg(ImmediateArg* arg) = 0; 
        virtual void VisitLabelArg(LabelArg* arg) = 0; 
};