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

void json_loadauthors(wxString workingdir, std::vector<wxString>* authors)
{
    authors->clear();
    wxString filepath = workingdir + wxString("/authors/authors.json");
    if (!wxFileExists(filepath))
        filepath = workingdir + wxString("/roms/roms.json");
    if (!wxFileExists(filepath))
        return;
    nlohmann::json pagejson = json_fromfilepath(filepath);
    if (pagejson.contains("Authors") && pagejson["Authors"].is_array())
        for (const auto& author : pagejson["Authors"])
            if (author.is_string())
                authors->push_back(wxString::FromUTF8(author.get<std::string>()));
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
    json_save
    Load a list of roms from a JSON file and populate the
    rom tree
    @param  The working directory of the program
    @param  A pointer to the category list
==============================*/

bool json_save(wxString workingdir, std::vector<Category*>* categorylist_roms, const std::vector<wxString>& authors)
{
    wxTextFile out_proj(workingdir + wxString("/roms/roms.json"));
    nlohmann::json romjson = {};
    romjson["Categories"] = {};
    nlohmann::json authorjson = {};
    authorjson["Authors"] = nlohmann::json::array();
    for (const wxString& author : authors)
        authorjson["Authors"].push_back(author.ToUTF8());

    wxString authorsdir = workingdir + wxString("/authors");
    if (!wxDirExists(authorsdir) && !wxFileName::Mkdir(authorsdir, 0777, wxPATH_MKDIR_FULL))
    {
        wxMessageBox("Could not create the authors directory.", "Error saving authors", wxOK|wxICON_ERROR);
        return false;
    }
    wxTextFile out_authors(authorsdir + wxString("/authors.json"));
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

    // Handle roms
    for (Category* cat : *categorylist_roms)
    {
        std::string catstr = cat->foldername.ToStdString();
        romjson["Categories"][catstr] = {};
        romjson["Categories"][catstr]["Index"] = cat->index;
        romjson["Categories"][catstr]["DisplayName"] = cat->displayname.ToUTF8();
        romjson["Categories"][catstr]["Description"] = cat->description.ToUTF8();
        romjson["Categories"][catstr]["Pages"] = {};
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
            if (!wxDirExists(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/")))
            {
                wxFileName dir(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/"));
                dir.Mkdir(0777, wxPATH_MKDIR_FULL);
                dir.SetPath(workingdir + wxString("/roms/") + cat->foldername + wxString("/images/"));
                dir.Mkdir(0777, wxPATH_MKDIR_FULL);
                dir.SetPath(workingdir + wxString("/roms/") + cat->foldername + wxString("/icons/"));
                dir.Mkdir(0777, wxPATH_MKDIR_FULL);
                dir.SetPath(workingdir + wxString("/roms/") + cat->foldername + wxString("/downloads/"));
                dir.Mkdir(0777, wxPATH_MKDIR_FULL);
            }
            if (!projmd.Exists())
                projmd.Create();
            projmd.Clear();
            projmd.AddLine(proj->description);
            projmd.Write();
            projmd.Close();
        }
    }

    // Dump the JSONs to text files
    if (!out_proj.Exists())
        out_proj.Create();
    out_proj.Clear();
    out_proj.AddLine(wxString::FromUTF8(romjson.dump(4)));
    out_proj.Write();
    out_proj.Close();
    return true;
}