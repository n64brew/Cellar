/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include <algorithm>
#include <fstream>
#include "json.h"
#include "helper.h"
#include "include/json.hpp"
#include <wx/textfile.h>
#include <wx/msgdlg.h>

/*==============================
    json_fromfilepath
    Gets a JSON object from a filepath
    @param  The filepath of the JSON file
    @return The JSON object or NULL
==============================*/

nlohmann::json json_fromfilepath(wxString filepath)
{
    try
    {
        if (!wxFileExists(filepath))
        {
            wxTextFile newfile;
            wxMessageBox(wxString("JSON file at '"+filepath+"' does not exist. It will be created."), wxString("Error opening JSON"), wxICON_ERROR);
            newfile.Create(filepath);
            newfile.AddLine("{}");
            newfile.Write();
            newfile.Close();
        }
        std::ifstream stream(filepath.mb_str());
        if (stream.is_open())
            return nlohmann::json::parse(stream);
    }
    catch (nlohmann::json::exception e)
    {
        wxMessageBox(wxString(e.what()), wxString("Error parsing JSON"), wxICON_ERROR);
    }
    return NULL;
}


/*==============================
    json_loadcategories
    Load a list of categories from a JSON file
    @param  The filepath of the JSON file
    @param  A pointer to the category list
    @return The number of categories loaded
==============================*/

int json_loadcategories(wxString filepath, std::vector<Category*>* categorylist)
{
    int index = 0;
    nlohmann::json pagejson = json_fromfilepath(filepath);

    // Add all the folders which are already included in the JSON
    for (nlohmann::json::iterator it = pagejson["Categories"].begin(); it != pagejson["Categories"].end(); ++it)
    {
        Category* cat;
        wxString catfoldername = wxString(it.key());

        // Create the category object
        cat = new Category();
        cat->foldername = catfoldername;
        cat->index = (*it)["Index"];
        cat->displayname = wxString(std::string((*it)["DisplayName"]));
        cat->description = wxString(std::string((*it)["Description"]));
        cat->wasmodified = false;
        cat->treeid = NULL;
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
    Load a list of roms from a JSON file and populate the
    rom tree
    @param  The working directory of the program
    @param  A pointer to the category list
    @param  A pointer to tree to populate
==============================*/

void json_loadroms(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree)
{
    nlohmann::json pagejson = json_fromfilepath(wxString(workingdir + wxString("/roms/roms.json")));

    // Add the roms from the JSON file
    for (nlohmann::json::iterator itcat = pagejson["Categories"].begin(); itcat != pagejson["Categories"].end(); ++itcat)
    {
        int index = 0;
        wxTreeItemId cat_id;
        Category* cat_elem = NULL;
        std::vector<ROM*> roms;

        // Find the category tree index
        for (Category* cat : *categorylist)
        {
            if (wxString(itcat.key()) == cat->foldername)
            {
                cat_elem = cat;
                cat_id = cat->treeid;
                break;
            }
        }

        // Read the rom data from the JSON
        for (nlohmann::json::iterator itproj = (*itcat)["Pages"].begin(); itproj != (*itcat)["Pages"].end(); ++itproj)
        {
            wxString taglist = wxString("");
            ROM* proj = new ROM();
            proj->index = (*itproj)["Index"];
            proj->filename = wxString::FromUTF8(itproj.key());
            proj->displayname = wxString::FromUTF8(std::string((*itproj)["DisplayName"]));
            proj->icon = wxString::FromUTF8(std::string((*itproj)["Icon"]));
            proj->date = (*itproj)["Date"];
            proj->tooltip = wxString::FromUTF8(std::string((*itproj)["ToolTip"]));
            proj->agerating = wxString::FromUTF8(std::string((*itproj)["AgeRating"]));
            proj->contentwarning = wxString::FromUTF8(std::string((*itproj)["ContentWarning"]));
            proj->aidisclosure = wxString::FromUTF8(std::string((*itproj)["AIDisclosure"]));
            proj->wasmodified = false;
            proj->description = string_fromfile(workingdir + wxString("/roms/") + cat_elem->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            proj->images.clear();
            for (nlohmann::json::iterator it = (*itproj)["Images"].begin(); it != (*itproj)["Images"].end(); ++it)
                proj->images.push_back(wxString::FromUTF8(std::string(*it)));
            proj->urls.clear();
            for (nlohmann::json::iterator it = (*itproj)["URLs"].begin(); it != (*itproj)["URLs"].end(); ++it)
                proj->urls.push_back(wxString::FromUTF8(std::string(*it)));
            proj->tags.clear();
            for (nlohmann::json::iterator it = (*itproj)["Tags"].begin(); it != (*itproj)["Tags"].end(); ++it)
                proj->tags.push_back(wxString::FromUTF8(std::string(*it)));
            proj->genres.clear();
            for (nlohmann::json::iterator it = (*itproj)["Genres"].begin(); it != (*itproj)["Genres"].end(); ++it)
                proj->genres.push_back(wxString::FromUTF8(std::string(*it)));
            if (itproj->contains("Authors") && (*itproj)["Authors"].is_array())
                for (const auto& author : (*itproj)["Authors"])
                    proj->authors.push_back(wxString::FromUTF8(author.get<std::string>()));
            proj->category = cat_elem;
            proj->treeid = NULL;
            roms.push_back(proj);
        }

        // Correct the indices of the roms, to make sure they're in proper order
        if (roms.size() > 0)
        {
            index = 0;
            std::sort(roms.begin(), roms.end(), &rom_sorter);
            for (ROM* proj : roms)
                proj->index = index++;

            // Now add the rom to the tree
            for (ROM* proj : roms)
            {
                proj->treeid = tree->AppendItem(cat_id, proj->displayname);
                cat_elem->pages.push_back(proj);
            }
        }
    }
}


/*==============================
    json_loadauthors
    Loads the registered authors from the JSON file
    @param The working directory of the program
    @param A pointer to the author list
==============================*/

void json_loadauthors(wxString workingdir, std::vector<Author*>* authors)
{
    nlohmann::json pagejson = json_fromfilepath(workingdir + wxString("/authors/authors.json"));

    // Add authors from the JSON file
    for (nlohmann::json::iterator itauthor = pagejson["Authors"].begin(); itauthor != pagejson["Authors"].end(); ++itauthor)
    {
        Author* author = new Author();
        author->filename = wxString::FromUTF8(itauthor.key());
        author->oldfilename = author->filename;
        author->displayname = wxString::FromUTF8(itauthor.value().value("DisplayName", itauthor.key()));
        author->image = wxString::FromUTF8(itauthor.value().value("Image", std::string()));
        wxString markdown = workingdir + "/authors/markdown/" + author->filename + ".md";
        if (wxFileExists(markdown))
            author->description = string_fromfile(markdown);
        authors->push_back(author);
    }
}


/*==============================
    json_save
    Save the ROM and author data to JSON and markdown files
    @param  The working directory of the program
    @param  A pointer to the category list
    @param  A pointer to the author list
    @return Whether the save worked or not
==============================*/

bool json_save(wxString workingdir, std::vector<Category*>* categorylist_roms, std::vector<Author*>* authors)
{
    wxTextFile out_proj(workingdir + wxString("/roms/roms.json"));
    wxTextFile out_authors(workingdir + wxString("/authors/authors.json"));
    nlohmann::json romjson = {};
    romjson["Categories"] = {};
    nlohmann::json authorjson = {};

    // Build author credits from the ROM assignments
    if (!wxDirExists(workingdir + "/authors"))
        wxFileName::Mkdir(workingdir + "/authors", 0777, wxPATH_MKDIR_FULL);
    authorjson["Authors"] = {};
    for (Author* author : *authors)
    {
        std::string key = author->filename.ToUTF8().data();
        authorjson["Authors"][key]["DisplayName"] = author->displayname.ToUTF8();
        authorjson["Authors"][key]["Image"] = author->image.ToUTF8();
        authorjson["Authors"][key]["Credits"] = {};
        for (Category* category : *categorylist_roms)
        {
            for (void* child : category->pages)
            {
                ROM* rom = (ROM*)child;
                if (std::find(rom->authors.begin(), rom->authors.end(), author->displayname) != rom->authors.end())
                    authorjson["Authors"][key]["Credits"].push_back((category->foldername + "/" + rom->filename).ToUTF8());
            }
        }
    }

    // Handle authors markdown files
    if (!wxDirExists(workingdir + "/authors/markdown"))
        wxFileName::Mkdir(workingdir + wxString("/authors/markdown"), 0777, wxPATH_MKDIR_FULL);
    for (Author* author : *authors)
    {
        wxTextFile markdown(workingdir + wxString("/authors/markdown") + "/" + author->filename + ".md");
        if (!(markdown.Exists() ? markdown.Open() : markdown.Create()))
        {
            wxMessageBox("Could not open an author description for saving.", "Error saving authors", wxOK|wxICON_ERROR);
            return false;
        }
        markdown.Clear();
        markdown.AddLine(author->description);
        if (!markdown.Write())
        {
            wxMessageBox("Could not save an author description.", "Error saving authors", wxOK|wxICON_ERROR);
            return false;
        }
        markdown.Close();
    }

    // Handle roms
    for (Category* cat : *categorylist_roms)
    {
        std::string catstr = cat->foldername.ToStdString();
        romjson["Categories"][catstr] = {};
        romjson["Categories"][catstr]["Index"] = cat->index;
        romjson["Categories"][catstr]["DisplayName"] = cat->displayname.ToUTF8();
        romjson["Categories"][catstr]["Description"] = cat->description.ToUTF8();
        romjson["Categories"][catstr]["Pages"] = {};
        if (!wxDirExists(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/")))
        {
            wxFileName::Mkdir(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/"), 0777, wxPATH_MKDIR_FULL);
            wxFileName::Mkdir(workingdir + wxString("/roms/") + cat->foldername + wxString("/images/"), 0777, wxPATH_MKDIR_FULL);
            wxFileName::Mkdir(workingdir + wxString("/roms/") + cat->foldername + wxString("/icons/"), 0777, wxPATH_MKDIR_FULL);
            wxFileName::Mkdir(workingdir + wxString("/roms/") + cat->foldername + wxString("/downloads/"), 0777, wxPATH_MKDIR_FULL);
        }
        for (void* child : cat->pages)
        {
            ROM* proj = (ROM*)child;
            std::string projstr = proj->filename.ToStdString();
            wxTextFile projmd(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            romjson["Categories"][catstr]["Pages"][projstr] = {};
            romjson["Categories"][catstr]["Pages"][projstr]["Index"] = proj->index;
            romjson["Categories"][catstr]["Pages"][projstr]["DisplayName"] = proj->displayname.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["Icon"] = proj->icon.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["Date"] = proj->date;
            romjson["Categories"][catstr]["Pages"][projstr]["ToolTip"] = proj->tooltip.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["AgeRating"] = proj->agerating.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["ContentWarning"] = proj->contentwarning.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["AIDisclosure"] = proj->aidisclosure.ToUTF8();
            romjson["Categories"][catstr]["Pages"][projstr]["Images"] = {};
            for (wxString str :  proj->images)
                romjson["Categories"][catstr]["Pages"][projstr]["Images"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][projstr]["URLs"] = {};
            for (wxString str :  proj->urls)
                romjson["Categories"][catstr]["Pages"][projstr]["URLs"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][projstr]["Tags"] = {};
            for (wxString str :  proj->tags)
                romjson["Categories"][catstr]["Pages"][projstr]["Tags"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][projstr]["Genres"] = {};
            for (wxString str :  proj->genres)
                romjson["Categories"][catstr]["Pages"][projstr]["Genres"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][projstr]["Authors"] = nlohmann::json::array();
            for (const wxString& author : proj->authors)
                romjson["Categories"][catstr]["Pages"][projstr]["Authors"].push_back(author.ToUTF8());
            if (!projmd.Exists())
                projmd.Create();
            projmd.Clear();
            projmd.AddLine(proj->description);
            projmd.Write();
            projmd.Close();
        }
    }

    // Dump the JSONs to text files
    if (!(out_proj.Exists() ? out_proj.Open() : out_proj.Create()))
    {
        wxMessageBox("Could not open the ROM file for saving.", "Error saving ROMs", wxOK|wxICON_ERROR);
        return false;
    }
    out_proj.Clear();
    out_proj.AddLine(wxString::FromUTF8(romjson.dump(4)));
    if (!out_proj.Write())
    {
        wxMessageBox("Could not save the ROM file.", "Error saving ROMs", wxOK|wxICON_ERROR);
        return false;
    }
    out_proj.Close();
    if (!(out_authors.Exists() ? out_authors.Open() : out_authors.Create()))
    {
        wxMessageBox("Could not open the authors file for saving.", "Error saving authors", wxOK|wxICON_ERROR);
        return false;
    }
    out_authors.Clear();
    out_authors.AddLine(wxString::FromUTF8(authorjson.dump(4)));
    if (!out_authors.Write())
    {
        wxMessageBox("Could not save the authors file.", "Error saving authors", wxOK|wxICON_ERROR);
        return false;
    }
    out_authors.Close();
    return true;
}