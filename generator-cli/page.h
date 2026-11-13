#pragma once

#include <string>
#include <vector>

class Category;

class Page 
{
    private:

    protected:

    public:
        int index;
        std::string filename;
        std::string displayname;
        std::string description;

        virtual void Compile() = 0;
        virtual ~Page() = default;
};