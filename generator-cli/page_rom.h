#pragma once

#include <string>
#include <vector>
#include "page.h"

class Page_ROM : public Page 
{
    private:

    protected:

    public:
        time_t date;
        std::string icon;
        std::string tooltip;
        std::string agerating;
        std::string contentwarning;
        std::string aidisclosure;
        std::vector<std::string> authors;
        std::vector<std::string> images;
        std::vector<std::string> genres;
        std::vector<std::string> urls;
        std::vector<std::string> tags;
        Category* category;

        void Compile();
};