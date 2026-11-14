#pragma once

#include <string>
#include <vector>
#include "page.h"
#include "page_author.h"

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
        std::vector<Page_Author*> authors;
        std::vector<std::string> images;
        std::vector<std::string> genres;
        std::vector<std::string> urls;
        std::vector<std::string> tags;
        Category* category;

        void Compile();
};