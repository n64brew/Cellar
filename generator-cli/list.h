#pragma once

#include <string>
#include "category.h"

class ListPage 
{
    private:

    protected:

    public:
        std::vector<Category*>* pages;

        virtual void Compile() = 0;
        virtual ~ListPage() = default;
};