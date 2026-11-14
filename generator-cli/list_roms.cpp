/***************************************************************
                         list_roms.cpp

Handle the ROM list page.
***************************************************************/

#include <string.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "list_roms.h"
#include "page_rom.h"
#include "markdown.h"
#include "helper.h"
#include "json.h"

void ListPage_ROMs::Compile()
{
    const std::string outfile = "roms.html";
    std::string html_final = "";
    std::string html_categories = "";

    // Read the roms page template
    html_final = string_fromfile("templates/roms.html");

    // Generate the page from the section + rom templates
    for (Category* cat : *this->pages)
    {
        std::string html_roms = "";
        std::string relativepath = "roms/" + cat->foldername + "/";
        std::string desccopy = cat->description;
        const char* mdstr = desccopy.c_str(); // TODO?: Potentially need to sanitize strings before sending to md_html
        std::string desc;

        // If there's no roms in this category, skip it
        if (cat->pages.size() == 0)
            continue;

        // Generate the rom blocks
        for (Page* page_a : cat->pages)
        {
            Page_ROM* page = dynamic_cast<Page_ROM*>(page_a);
            std::string html_rom = string_fromfile("templates/roms_rom.html");
            strreplace_all(html_rom, "_TEMPLATE_ROM_URL_", sanitize_html(relativepath + page->filename + ".html"));
            strreplace_all(html_rom, "_TEMPLATE_ROM_TITLE_", sanitize_html(page->displayname));
            strreplace_all(html_rom, "_TEMPLATE_ROM_IMAGE_",  page->icon.empty() ? "roms/noicon.png" : relativepath + page->icon);
            strreplace_all(html_rom, "_TEMPLATE_ROM_TOOLTIP_", sanitize_html(page->tooltip));
            strreplace_all(html_rom, "_TEMPLATE_ROM_FILTER_", sanitize_html(json_romfilter(page)));
            html_roms += html_rom + "\r\n";
        }

        // Generate the section blocks
        html_categories += string_fromfile("templates/roms_section.html");
        strreplace_all(html_categories, "_TEMPLATE_TITLE_", cat->displayname);
        strreplace_all(html_categories, "_TEMPLATE_HREF_", cat->foldername);
        md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &desc, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS{});
        if (desc.length() == 0 && cat->description.length() > 0)
        {
            std::string err = "Markdown parsing was unsuccessful for '" + cat->displayname + "'! Do you have some unsupported unicode?";
            printf("%s\n", err.c_str());
            exit(4);
        }
        strreplace_all(desc, "<p>", "<p align=\"left\">");
        strreplace_all(html_categories, "_TEMPLATE_SECTION_DESCRIPTION_", desc);
        strreplace_all(html_categories, "_TEMPLATE_ROMS_LIST_", html_roms);
        html_categories += "\r\n";
    }

    // Finalize the roms page
    strreplace_all(html_final, "_TEMPLATE_ROMS_LIST_", html_categories);

    // Put the date
    std::time_t now = std::time(nullptr);
    std::tm today{};
    #ifdef _WIN32
        localtime_s(&today, &now);
    #else
        localtime_r(&now, &today);
    #endif
    std::ostringstream oss;
    oss << std::put_time(&today, "%b %d %Y");
    strreplace_all(html_final, "_TEMPLATE_ROMS_DATE_", oss.str());

    // Dump the html for the roms page
    std::ofstream out(outfile);
    out << html_final;
    out.close();
}