#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include "json.h"
#include "page.h"
#include "category.h"
#include "list_roms.h"
#include "helper.h"


/*********************************
           Enumerations
*********************************/

// Helper enum for the program mode
typedef enum
{
    MODE_SINGLE,
    MODE_ALL
} ProgramMode;


/*********************************
            Structures
*********************************/

// Helper struct for sorting tagged pages
//typedef struct TaggedPage_s
//{
//    PageType type;
//    void* page;
//} TaggedPage;


/*********************************
            Structures
*********************************/

void compile_roms(std::vector<Category*>* pages);
void compile_rom(std::vector<Category*>* pages, std::string jsonpath);
void compile_tags(std::vector<Category*>* pages);
void compile_homepage(std::vector<Category*>* pages);


/*==============================
    main
    Program entrypoint
    @param Argument count
    @param Argument values
==============================*/

int main(int argc, char* argv[])
{
    char* target = NULL;
    std::string contentfolder, jsonname;
    ProgramMode mode = MODE_ALL;
    nlohmann::json json;
    std::vector<Category*> pages;

    // Ensure we have some cli arguments
    if (argc < 2)
    {
        printf("Usage:\n");
        printf("    %s folder/to/read <options>\n\n", argv[0]);
        printf("Available options:\n");
        printf("-all            (default, assumed) Full recompile\n");
        printf("-single <name>  Compile a single page (in category/internalname format)\n");
        return 1;
    }

    // Read the CLI arguments
    contentfolder = argv[1];
    if (argc > 2)
    {
        for (int i=2; i<argc; i++)
        {
            if (!strcmp(argv[i], "-single"))
            {
                mode = MODE_SINGLE;
                i++;
                target = argv[i];

            }
            else if (!strcmp(argv[i], "-all"))
            {
                // Do nothing
            }
            else
            {
                printf("Unknown argument %s\n", argv[i]);
                return 2;
            }
        }
    }

    // Get the folder name and try to read the JSON file from it
    if (contentfolder.back() == '\\' || contentfolder.back() == '/')
        contentfolder.pop_back();
    std::size_t found = contentfolder.find_last_of("/\\");
    jsonname = contentfolder.substr(found+1);

    // Open the JSON file and load its content
    json_loadcategories(contentfolder + "/" + jsonname + ".json", &pages);
    json_loadroms(contentfolder + "/" + jsonname + ".json", &pages);

    // Compile the website
    if (jsonname == "roms")
    {
        if (mode == MODE_ALL)
            compile_roms(&pages);
        else
            compile_rom(&pages, target);
    }
    compile_tags(&pages);
    compile_homepage(&pages);
    return 0;
}


/*==============================
    compile_roms
    Compile all rom-related pages
    @param A pointer to the list of categories
==============================*/

void compile_roms(std::vector<Category*>* pages)
{
    // ROM list page
    ListPage* list = new ListPage_ROMs();
    list->pages = pages;
    list->Compile();

    // Now create the page for each rom
    for (Category* cat : *pages)
        for (Page* page : cat->pages)
            page->Compile();
}


/*==============================
    compile_rom
    Compile a single rom-related page
    @param A pointer to the list of categories
    @param The path to the page we want to compile
==============================*/

void compile_rom(std::vector<Category*>* pages, std::string jsonpath)
{
    size_t pos = jsonpath.find('/');
    std::string category = jsonpath.substr(0, pos);
    std::string pagename = jsonpath.substr(pos + 1);

    ListPage* list = new ListPage_ROMs();
    list->pages = pages;
    list->Compile();

    for (Category* cat : *pages)
    {
        if (cat->foldername == category)
        {
            for (Page* page : cat->pages)
            {
                if (page->filename == pagename)
                {
                    page->Compile();
                    return;
                }
            }
        }
    }
}


/*==============================
    compile_tags
    Compile the tag pages
    @param A pointer to the list of categories
==============================*/

void compile_tags(std::vector<Category*>* pages)
{
    /*
    bool cont;
    std::string filename;
    std::string tagspath = "tags";
    wxDir tagsdir(tagspath);
    std::map<std::string, std::vector<TaggedPage>> tagmap;

    // Go through all roms and add each one to the tag map
    for (Category* cat : this->m_Category_ROMs)
    {
        for (void* page : cat->pages)
        {
            for (std::string tag : ((ROM*)page)->tags)
            {
                std::map<std::string, std::vector<TaggedPage>>::iterator foundtag = tagmap.find(tag);
                if (foundtag == tagmap.end())
                    foundtag = tagmap.insert(std::pair<std::string, std::vector<TaggedPage>>(tag, std::vector<TaggedPage>())).first;
                foundtag->second.push_back({PAGETYPE_ROM, page});
            }
        }
    }

    // Purge the tags folder of html files
    cont = tagsdir.GetFirst(&filename, "*.html");
    while (cont)
    {
        wxRemoveFile(tagspath + "/" + filename);
        cont = tagsdir.GetNext(&filename);
    }

    // Now, alphabetically sort all the pages in the tags, and make their respective pages
    for (std::pair<std::string, std::vector<TaggedPage>> it : tagmap)
    {
        std::string tagname_page;
        std::string pagelist;
        std::string html_final = string_fromfile("templates/tag.html");
        std::sort(it.second.begin(), it.second.end(), taggedpage_sorter);

        // For a valid tag, its name needs to be sanitized
        tagname_page = sanitize_tagname(it.first);

        // Fill the tag page
        strreplace_all(html_final, "_TEMPLATE_TAG_NAME_", it.first);
        strreplace_all(html_final, "_TEMPLATE_TAG_CLEANNAME_", "tags/" + tagname_page + ".html");
        for (TaggedPage tp : it.second)
        {
            std::string item = string_fromfile("templates/tag_item.html");
            ROM* proj = (ROM*)tp.page;
            strreplace_all(item, "_TEMPLATE_TAG_PAGENAME_", proj->displayname);
            strreplace_all(item, "_TEMPLATE_TAG_PAGEURL_", "../roms/" + proj->category->foldername + "/" + proj->filename + ".html");
            pagelist += item + "\r\n";
        }
        strreplace_all(html_final, "_TEMPLATE_TAG_PAGELIST_", pagelist);

        // Generate the page itself
        std::ofstream out(tagspath + "/" + tagname_page + ".html");
        out << html_final;
        out.close();
    }
    */
}

/*==============================
    compile_homepage
    Compile the homepage
    @param A pointer to the list of categories
==============================*/

void compile_homepage(std::vector<Category*>* pages)
{
    int count;
    std::string homepath = "index.html";
    std::string html_entry;
    std::string html_final = string_fromfile("templates/index.html");
    std::vector<Page_ROM*> latestroms;

    // Get the list of roms with dates
    for (Category* cat : *pages)
    {
        for (Page* page : cat->pages)
        {
            Page_ROM* rom = dynamic_cast<Page_ROM*>(page);
            if (rom->date != "")
                latestroms.push_back(rom);
        }
    }    

    // Paste the 5 most recent roms
    count = 0;
    html_entry = "";
    std::sort(latestroms.begin(), latestroms.end(), &rom_sorter_date);
    for (Page_ROM* page : latestroms)
    {
        std::tm date;
        std::string tableelem;
        if (count == 5)
            break;

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

        // Replace
        tableelem = string_fromfile("templates/index_latest.html");
        if (date_fromstring(page->date, &date))
            strreplace_all(tableelem, "_TEMPLATE_LATEST_ENTRY_DATE_", oss.str());
        else
            strreplace_all(tableelem, "_TEMPLATE_LATEST_ENTRY_DATE_", "\?\?/\?\?/\?\?\?\?");
        strreplace_all(tableelem, "_TEMPLATE_LATEST_ENTRY_CATEGORY_", page->category->displayname);
        strreplace_all(tableelem, "_TEMPLATE_LATEST_ENTRY_NAME_", page->displayname);
        strreplace_all(tableelem, "_TEMPLATE_ENTRY_CATEGORY_", "roms.html#" + page->category->foldername);
        strreplace_all(tableelem, "_TEMPLATE_ENTRY_PAGE_", "roms/" + page->category->foldername + "/" + page->filename + ".html");
        html_entry += tableelem + "\r\n";
        count++;
    }
    strreplace_all(html_final, "_TEMPLATE_LATEST_ROMS_", html_entry);

    // Generate the page itself
    std::ofstream out(homepath);
    out << html_final;
    out.close();
}