/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include <fstream>
#include "page.h"
#include "page_rom.h"
#include "category.h"
#include "json.h"
#include "helper.h"
#include "include/json.hpp"


/*==============================
    json_fromfilepath
    Gets a JSON object from a filepath
    @param  The filepath of the JSON file
    @return The JSON object or NULL
==============================*/

static nlohmann::json json_fromfilepath(std::string filepath)
{
    try
    {
        std::ifstream stream(filepath);
        if (stream.is_open())
            return nlohmann::json::parse(stream);
    }
    catch (nlohmann::json::exception e)
    {
        printf("Error parsing JSON\n");
        exit(10);
    }
    return NULL;
}


/*==============================
    category_sorter
    Sorts two category objects by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

static bool category_sorter(Category* lhs, Category* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    page_sorter
    Sorts two pages by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

static bool page_sorter(Page* lhs, Page* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    json_loadcategories
    Load a list of categories from a JSON file
    @param  The path to the JSON file
    @param  A pointer to the category list
    @return The number of categories loaded
==============================*/

int json_loadcategories(std::string filepath, std::vector<Category*>* categorylist)
{
    int index = 0;
    nlohmann::json json = json_fromfilepath(filepath);

    // Add all the folders which are already included in the JSON
    for (nlohmann::json::iterator it = json["Categories"].begin(); it != json["Categories"].end(); ++it)
    {
        Category* cat;
        std::string catfoldername = it.key();

        // Create the category object
        cat = new Category();
        cat->foldername = catfoldername;
        cat->index = (*it)["Index"];
        cat->displayname = std::string((*it)["DisplayName"]);
        cat->description = std::string((*it)["Description"]);
        cat->pages.clear();
        categorylist->push_back(cat);
    }

    // Correct the indices of categories, to make sure they're in proper order
    std::sort(categorylist->begin(), categorylist->end(), &category_sorter);
    for (Category* cat : *categorylist)
        cat->index = index++;

    // Return the number of category elements we found
    return index;
}


/*==============================
    json_loadroms
    Load a list of roms from a JSON file
    @param  The path to the JSON file
    @param A pointer to the category list
    @param A pointer to the author list
    @param A pointer to the unregistered author list
==============================*/

void json_loadroms(std::string filepath, std::vector<Category*>* categorylist, const std::vector<Page_Author*>* authors, std::vector<std::unique_ptr<Page_Author>>* unknownauthors)
{
    std::size_t found = filepath.find_last_of("/\\");
    std::string workingdir = filepath.substr(0, found);
    nlohmann::json json = json_fromfilepath(filepath);

    // Add the roms from the JSON file
    for (nlohmann::json::iterator itcat = json["Categories"].begin(); itcat != json["Categories"].end(); ++itcat)
    {
        int index = 0;
        Category* cat_elem = NULL;
        std::vector<Page*> pages;

        // Find the category tree index
        for (Category* cat : *categorylist)
        {
            if (std::string(itcat.key()) == cat->foldername)
            {
                cat_elem = cat;
                break;
            }
        }

        // Read the rom data from the JSON
        for (nlohmann::json::iterator itproj = (*itcat)["Pages"].begin(); itproj != (*itcat)["Pages"].end(); ++itproj)
        {
            std::string taglist = std::string("");
            Page_ROM* page = new Page_ROM();
            page->index = (*itproj)["Index"];
            page->category = cat_elem;
            page->filename = std::string(itproj.key());
            page->displayname = std::string((*itproj)["DisplayName"]);
            page->icon = std::string((*itproj)["Icon"]);
            page->date = (*itproj)["Date"];
            page->tooltip = std::string((*itproj)["ToolTip"]);
            page->agerating = std::string((*itproj)["AgeRating"]);
            page->contentwarning = std::string((*itproj)["ContentWarning"]);
            page->aidisclosure = std::string((*itproj)["AIDisclosure"]);
            page->description = string_fromfile(workingdir + "/" + cat_elem->foldername + "/markdown/" + page->filename + ".md");
            page->images.clear();
            for (nlohmann::json::iterator it = (*itproj)["Images"].begin(); it != (*itproj)["Images"].end(); ++it)
                page->images.push_back(std::string(*it));
            page->urls.clear();
            for (nlohmann::json::iterator it = (*itproj)["URLs"].begin(); it != (*itproj)["URLs"].end(); ++it)
                page->urls.push_back(std::string(*it));
            page->tags.clear();
            for (nlohmann::json::iterator it = (*itproj)["Tags"].begin(); it != (*itproj)["Tags"].end(); ++it)
                page->tags.push_back(std::string(*it));
            page->genres.clear();
            for (nlohmann::json::iterator it = (*itproj)["Genres"].begin(); it != (*itproj)["Genres"].end(); ++it)
                page->genres.push_back(std::string(*it));
            page->authors.clear();
            for (nlohmann::json::iterator it = (*itproj)["Authors"].begin(); it != (*itproj)["Authors"].end(); ++it)
            {
                std::string name = std::string(*it);
                Page_Author* author = NULL;
                for (Page_Author* candidate : *authors)
                    if (candidate->displayname == name)
                    {
                        author = candidate;
                        break;
                    }
                if (author == NULL)
                    for (const std::unique_ptr<Page_Author>& candidate : *unknownauthors)
                        if (candidate->displayname == name)
                        {
                            author = candidate.get();
                            break;
                        }
                if (author == NULL)
                {
                    unknownauthors->emplace_back(new Page_Author());
                    author = unknownauthors->back().get();
                    author->displayname = name;
                    author->filename = sanitize_tagname(name);
                }
                page->authors.push_back(author);
            }
            pages.push_back(page);
        }

        // Correct the indices of the roms, to make sure they're in proper order
        if (pages.size() > 0)
        {
            index = 0;
            std::sort(pages.begin(), pages.end(), &page_sorter);
            for (Page* page : pages)
                page->index = index++;

            // Now add the rom to the category list
            for (Page* page : pages)
                cat_elem->pages.push_back(page);
        }
    }
}


/*==============================
    json_loadauthors
    Load the authors from a JSON file
    @param The path to the JSON file
    @param A pointer to the author list
==============================*/

void json_loadauthors(std::string filepath, std::vector<Page_Author*>* authors)
{
    std::size_t found = filepath.find_last_of("/\\");
    std::string workingdir = filepath.substr(0, found);
    nlohmann::json json = json_fromfilepath(filepath);
    if (!json.is_object() || !json.contains("Authors") || !json["Authors"].is_object())
        return;

    // Add the authors from the JSON file
    for (nlohmann::json::iterator it = json["Authors"].begin(); it != json["Authors"].end(); ++it)
    {
        if (!it.value().is_object())
            continue;
        Page_Author* author = new Page_Author();
        author->filename = it.key();
        author->displayname = it.value().value("DisplayName", author->filename);
        author->image = it.value().value("Image", std::string());
        author->description = string_fromfile(workingdir + "/markdown/" + author->filename + ".md");
        if (it.value().contains("Credits") && it.value()["Credits"].is_array())
            for (const nlohmann::json& credit : it.value()["Credits"])
                if (credit.is_string())
                    author->credits.push_back(credit.get<std::string>());
        authors->push_back(author);
    }
}