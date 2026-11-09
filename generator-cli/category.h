#pragma once

#include <string>
#include <vector>
#include "page.h"

class Category
{
    private:

    protected:

    public:
        int index;
        std::string foldername;
        std::string displayname;
        std::string description;
        std::vector<Page*> pages;
};