#pragma once

#include <assert.h>
#include <cstdlib>
#include <memory>

class Assembler
{
    public:
        virtual ~Assembler() = default; 

        virtual void AssembleProgram(const std::string& filepath, const std::string& outputpath) = 0; 
};

class GCCAssembler : public Assembler
{
    public:
        void AssembleProgram(const std::string& filepath, const std::string& outputpath) override
        {
            auto gccCmd = "gcc " + filepath + " -o " + outputpath;
            system(gccCmd.c_str()); 
            if (true)
            {
                //auto rmCmd = "rm " + filepath; 
                //system(rmCmd.c_str());
            }
        }
};

// generate assembler based on architecture
static std::shared_ptr<Assembler> CreateAssembler()
{
    assert("not yet implemented");
}