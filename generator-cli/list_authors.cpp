/***************************************************************
                        list_authors.cpp

Handle the author list page.
***************************************************************/

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "list_authors.h"
#include "helper.h"


/*==============================
    Compile
    Compile the flat author list
==============================*/

void ListPage_Authors::Compile()
{
    std::string html_final = string_fromfile("templates/authors.html");
    std::string html_authors;
    std::sort(this->authors->begin(), this->authors->end(), &author_sorter);

    // Generate the author blocks
    for (Page_Author* author : *this->authors)
    {
        std::string item = string_fromfile("templates/authors_author.html");
        strreplace_all(item, "_TEMPLATE_AUTHOR_URL_", "authors/" + author->filename + ".html");
        strreplace_all(item, "_TEMPLATE_AUTHOR_TITLE_", sanitize_html(author->displayname));
        strreplace_all(item, "_TEMPLATE_AUTHOR_IMAGE_", author->image.empty() ? "authors/noimage.png" : "authors/" + author->image);
        html_authors += item + "\r\n";
    }
    strreplace_all(html_final, "_TEMPLATE_AUTHORS_LIST_", html_authors);

    // Add the current date
    std::time_t now = std::time(nullptr);
    std::tm today{};
    #ifdef _WIN32
        localtime_s(&today, &now);
    #else
        localtime_r(&now, &today);
    #endif
    std::ostringstream date;
    date << std::put_time(&today, "%b %d %Y");
    strreplace_all(html_final, "_TEMPLATE_AUTHORS_DATE_", date.str());

    // Write the HTML file
    std::ofstream out("authors.html");
    out << html_final;
    out.close();
}