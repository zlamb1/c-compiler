#pragma once

struct RegisterArg;
struct ImmediateArg; 

class SyntaxVisitor
{
    public:
        virtual std::string VisitRegisterArg(RegisterArg* arg) = 0;
        virtual std::string VisitImmediateArg(ImmediateArg* arg) = 0; 
};