/***************************************************************
                        page_author.cpp

Handle the individual author page.
***************************************************************/

#include <string.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "category.h"
#include "page_author.h"
#include "markdown.h"
#include "helper.h"


/*==============================
    Compile
    Compile an individual author page
==============================*/

void Page_Author::Compile()
{
    std::string html_final = string_fromfile("templates/author.html");
    std::string html_image;
    std::string html_description;
    std::string html_credits;

    // Generate the author image and markdown description
    if (!image.empty())
        html_image = "<img class=\"authorimage\" src=\"" + this->image + "\" alt=\"" + sanitize_html(this->displayname) + "\">";
    if (!this->description.empty())
    {
        MD_TOC_OPTIONS options = {};
        md_html(this->description.c_str(), this->description.length(), md4c_funcptr_handlestr, &html_description, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, &options);
        if (html_description.empty())
        {
            printf("Markdown parsing was unsuccessful for '%s'!\n", this->displayname.c_str());
            exit(5);
        }
    }

    // Resolve the credited ROM identifiers to page names and URLs
    if (this->categories != NULL)
    {
        for (std::string credit : this->credits)
        {
            for (Category* category : *this->categories)
            {
                for (Page* page : category->pages)
                {
                    if (category->foldername + "/" + page->filename == credit)
                    {
                        std::string item = string_fromfile("templates/tag_item.html");
                        strreplace_all(item, "_TEMPLATE_TAG_PAGEURL_", "../roms/" + sanitize_html(credit) + ".html");
                        strreplace_all(item, "_TEMPLATE_TAG_PAGENAME_", sanitize_html(page->displayname));
                        html_credits += item + "\r\n";
                    }
                }
            }
        }
    }
    if (!html_credits.empty())
        html_credits = "<h2>Credits</h2>\n" + html_credits;

    // Fill the author template and write the page
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_NAME_", sanitize_html(this->displayname));
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_URL_", "authors/" + this->filename + ".html");
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_IMAGE_", "authors/" + image);
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_IMAGEBLOCK_", html_image);
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_DESCRIPTION_", html_description);
    strreplace_all(html_final, "_TEMPLATE_AUTHOR_CREDITS_", html_credits);
    std::ofstream out("authors/" + this->filename + ".html");
    out << html_final;
    out.close();
}