#pragma once

#include <string>
#include <vector>
#include <map>
#include "page.h"

class Page_Author : public Page 
{
    private:

    protected:

    public:
        std::string image;
        std::vector<std::string> credits;
        std::vector<Category*>* categories = NULL;
        
        void Compile();
};