#pragma once

#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <iostream>

#include "compiler/flags.hpp"

class CodeGenerator
{
    public:
        CodeGenerator() = default;
        virtual ~CodeGenerator() = default; 

        void SetOutputStream(std::shared_ptr<std::ostream> outputstream)
        {
            m_OutputStream = outputstream; 
        }
        
    protected: 
        std::shared_ptr<std::ostream> m_OutputStream = nullptr; 

        std::ostream& outputstream()
        {
            return *(m_OutputStream.get()); 
        }
};