#pragma once

#include "list.h"
#include "page_author.h"

class ListPage_Authors : public ListPage
{
    private:

    protected:

    public:
        std::vector<Page_Author*>* authors;

        void Compile();
};