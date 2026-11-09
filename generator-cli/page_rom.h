#pragma once

#include <string>
#include <vector>
#include "page.h"

class Page_ROM : public Page 
{
    private:

    protected:

    public:
        std::vector<std::string> images;
        std::vector<std::string> urls;

        void Compile();
};