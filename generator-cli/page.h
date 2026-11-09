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
        std::string icon;
        std::string date;
        std::string tooltip;
        std::string description;
        std::vector<std::string> tags;
        Category* category;

        virtual void Compile() = 0;
        virtual ~Page() = default;
};