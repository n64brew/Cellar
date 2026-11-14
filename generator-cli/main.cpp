/***************************************************************
                            main.cpp

The main code of the static site generator compiler
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include "json.h"
#include "page.h"
#include "category.h"
#include "list_roms.h"
#include "list_authors.h"
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
       Function Prototypes
*********************************/

void compile_roms(std::vector<Category*>* pages);
void compile_rom(std::vector<Category*>* pages, std::string jsonpath);
void compile_authors(std::vector<Page_Author*>* authors);
void compile_author(std::vector<Page_Author*>* authors, std::string jsonpath);
void compile_tags(std::vector<Category*>* pages);
void compile_genres(std::vector<Category*>* pages);
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
    ProgramMode mode = MODE_ALL;
    std::string rompath = "roms/roms.json";
    std::string authorpath = "authors/authors.json";
    std::string contentfolder, jsonname;
    nlohmann::json json;
    std::vector<Category*> pages;
    std::vector<Page_Author*> authors;
    std::vector<std::unique_ptr<Page_Author>> unknownauthors;

    // Ensure we have some cli arguments
    if (argc < 2)
    {
        printf("Usage:\n");
        printf("    %s all|roms|authors <options>\n\n", argv[0]);
        printf("Available options:\n");
        printf("-all            (default, assumed) Compile all pages of the selected type\n");
        printf("-single <name>  Compile one ROM (category/internalname) or author (filename)\n");
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
                if (i + 1 >= argc)
                {
                    printf("Missing page name after -single\n");
                    return 2;
                }
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
    if (!contentfolder.empty() && (contentfolder.back() == '\\' || contentfolder.back() == '/'))
        contentfolder.pop_back();
    std::size_t found = contentfolder.find_last_of("/\\");
    jsonname = contentfolder.substr(found+1);
    if (jsonname != "all" && jsonname != "roms" && jsonname != "authors")
    {
        printf("Unknown target %s (expected 'all', 'roms', or 'authors')\n", contentfolder.c_str());
        return 2;
    }
    if (jsonname == "all" && mode == MODE_SINGLE)
    {
        printf("-single requires the roms or authors target\n");
        return 2;
    }

    // Load the ROMs and the authors
    json_loadcategories(rompath, &pages);
    json_loadauthors(authorpath, &authors);
    json_loadroms(rompath, &pages, &authors, &unknownauthors);
    for (Page_Author* author : authors)
        author->categories = &pages;

    // Compile the website
    if (jsonname == "all")
    {
        compile_roms(&pages);
        compile_authors(&authors);
        compile_tags(&pages);
        compile_genres(&pages);
        compile_homepage(&pages);
    }
    else if (jsonname == "roms")
    {
        if (mode == MODE_ALL)
            compile_roms(&pages);
        else
            compile_rom(&pages, target);
    }
    else if (jsonname == "authors")
    {
        if (mode == MODE_ALL)
            compile_authors(&authors);
        else
            compile_author(&authors, target);
    }

    printf("Site compiled successfully.\n");
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
    printf("ROM '%s' not found\n", jsonpath.c_str());
    exit(8);
}


/*==============================
    compile_authors
    Compile the author list and author pages
    @param A pointer to the author list
==============================*/

void compile_authors(std::vector<Page_Author*>* authors)
{
    // Author list page
    ListPage_Authors* list = new ListPage_Authors();
    list->authors = authors;
    list->Compile();
    delete list;

    // Now create the page for each author
    makedir("authors");
    for (Page_Author* author : *authors)
        author->Compile();
}


/*==============================
    compile_author
    Compile a single author page
    @param A pointer to the author list
    @param The filename of a single author, or an empty string
==============================*/

void compile_author(std::vector<Page_Author*>* authors, std::string jsonpath)
{
    ListPage_Authors* list = new ListPage_Authors();
    list->authors = authors;
    list->Compile();
    delete list;

    makedir("authors");
    for (Page_Author* author : *authors)
    {
        if (author->filename == jsonpath)
        {
            author->Compile();
            return;
        }
    }
    printf("Author '%s' not found\n", jsonpath.c_str());
    exit(9);
}


/*==============================
    compile_tags
    Compile the tag pages
    @param A pointer to the list of categories
==============================*/

void compile_tags(std::vector<Category*>* pages)
{
    std::string filename;
    std::string tagspath = "tags";
    std::map<std::string, std::vector<Page*>> tagmap;

    // Go through all pages and add each one to the tag map
    for (Category* cat : *pages)
    {
        for (Page* page : cat->pages)
        {
            if (dynamic_cast<Page_ROM*>(page))
            {
                Page_ROM* rom = dynamic_cast<Page_ROM*>(page);
                for (std::string tag : rom->tags)
                {
                    std::map<std::string, std::vector<Page*>>::iterator foundtag = tagmap.find(tag);
                    if (foundtag == tagmap.end())
                        foundtag = tagmap.insert(std::pair<std::string, std::vector<Page*>>(tag, std::vector<Page*>())).first;
                    foundtag->second.push_back(rom);
                }
            }
        }
    }

    // Purge the tags folder of html files
    makedir(tagspath);
    purge_htmlfiles(tagspath);

    // Now, alphabetically sort all the pages in the tags, and make their respective pages
    for (std::pair<std::string, std::vector<Page*>> it : tagmap)
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
        for (Page* page : it.second)
        {
            std::string item = string_fromfile("templates/tag_item.html");
            if (dynamic_cast<Page_ROM*>(page))
            {
                Page_ROM* rom = dynamic_cast<Page_ROM*>(page);
                strreplace_all(item, "_TEMPLATE_TAG_PAGENAME_", rom->displayname);
                strreplace_all(item, "_TEMPLATE_TAG_PAGEURL_", "../roms/" + rom->category->foldername + "/" + rom->filename + ".html");
            }
            pagelist += item + "\r\n";
        }
        strreplace_all(html_final, "_TEMPLATE_TAG_PAGELIST_", pagelist);

        // Generate the page itself
        std::ofstream out(tagspath + "/" + tagname_page + ".html");
        out << html_final;
        out.close();
    }
}


/*==============================
    compile_genres
    Compile the genres pages
    @param A pointer to the list of categories
==============================*/

void compile_genres(std::vector<Category*>* pages)
{
    std::string filename;
    std::string genrespath = "genre";
    std::map<std::string, std::vector<Page*>> genremap;

    // Go through all pages and add each one to the genre map
    for (Category* cat : *pages)
    {
        for (Page* page : cat->pages)
        {
            if (dynamic_cast<Page_ROM*>(page))
            {
                Page_ROM* rom = dynamic_cast<Page_ROM*>(page);
                for (std::string genre : rom->genres)
                {
                    std::map<std::string, std::vector<Page*>>::iterator foundgenre = genremap.find(genre);
                    if (foundgenre == genremap.end())
                        foundgenre = genremap.insert(std::pair<std::string, std::vector<Page*>>(genre, std::vector<Page*>())).first;
                    foundgenre->second.push_back(rom);
                }
            }
        }
    }

    // Purge the genres folder of html files
    makedir(genrespath);
    purge_htmlfiles(genrespath);

    // Now, alphabetically sort all the pages in the tags, and make their respective pages
    for (std::pair<std::string, std::vector<Page*>> it : genremap)
    {
        std::string genrename_page;
        std::string pagelist;
        std::string html_final = string_fromfile("templates/genre.html");
        std::sort(it.second.begin(), it.second.end(), taggedpage_sorter);

        // For a valid genre, its name needs to be sanitized
        genrename_page = sanitize_tagname(it.first);

        // Fill the genre page
        strreplace_all(html_final, "_TEMPLATE_GENRE_NAME_", it.first);
        strreplace_all(html_final, "_TEMPLATE_GENRE_CLEANNAME_", "genres/" + genrename_page + ".html");
        for (Page* page : it.second)
        {
            std::string item = string_fromfile("templates/genre_item.html");
            if (dynamic_cast<Page_ROM*>(page))
            {
                Page_ROM* rom = dynamic_cast<Page_ROM*>(page);
                strreplace_all(item, "_TEMPLATE_GENRE_PAGENAME_", rom->displayname);
                strreplace_all(item, "_TEMPLATE_GENRE_PAGEURL_", "../roms/" + rom->category->foldername + "/" + rom->filename + ".html");
            }
            pagelist += item + "\r\n";
        }
        strreplace_all(html_final, "_TEMPLATE_GENRE_PAGELIST_", pagelist);

        // Generate the page itself
        std::ofstream out(genrespath + "/" + genrename_page + ".html");
        out << html_final;
        out.close();
    }
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
            if (rom->date != 0)
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

        // Replace
        tableelem = string_fromfile("templates/index_latest.html");
        if (page->date != 0)
            strreplace_all(tableelem, "_TEMPLATE_LATEST_ENTRY_DATE_", date_tostring(page->date));
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