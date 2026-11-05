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
        if (wxFileExists(filepath))
        {
            std::ifstream stream(filepath.mb_str());
            if (stream.is_open())
                return nlohmann::json::parse(stream);
        }
        else
            wxMessageBox(wxString("JSON file at '"+filepath+"' does not exist."), wxString("Error opening JSON"), wxICON_ERROR);
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

        // If the category folder doesn't exist anymore, skip this category
        if (!wxDirExists(wxFileName(filepath).GetPath() + wxString("/") + catfoldername))
            continue;

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
    json_loadprojects
    Load a list of projects from a JSON file and populate the
    project tree
    @param  The working directory of the program
    @param  A pointer to the category list
    @param  A pointer to tree to populate
==============================*/

void json_loadprojects(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree)
{
    nlohmann::json pagejson = json_fromfilepath(wxString(workingdir + wxString("/roms.json")));

    // Add the projects from the JSON file
    for (nlohmann::json::iterator itcat = pagejson["Categories"].begin(); itcat != pagejson["Categories"].end(); ++itcat)
    {
        int index = 0;
        wxTreeItemId cat_id;
        Category* cat_elem = NULL;
        std::vector<Project*> projects;

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

        // Read the project data from the JSON
        for (nlohmann::json::iterator itproj = (*itcat)["Pages"].begin(); itproj != (*itcat)["Pages"].end(); ++itproj)
        {
            wxString taglist = wxString("");
            Project* proj = new Project();
            proj->index = (*itproj)["Index"];
            proj->filename = wxString::FromUTF8(itproj.key());
            proj->displayname = wxString::FromUTF8(std::string((*itproj)["DisplayName"]));
            proj->icon = wxString::FromUTF8(std::string((*itproj)["Icon"]));
            proj->date = wxString::FromUTF8(std::string((*itproj)["Date"]));
            proj->tooltip = wxString::FromUTF8(std::string((*itproj)["ToolTip"]));
            proj->wasmodified = false;
            proj->description = string_fromfile(workingdir + wxString("/projects/") + cat_elem->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            proj->images.clear();
            for (nlohmann::json::iterator it = (*itproj)["Images"].begin(); it != (*itproj)["Images"].end(); ++it)
                proj->images.push_back(wxString::FromUTF8(std::string(*it)));
            proj->urls.clear();
            for (nlohmann::json::iterator it = (*itproj)["URLs"].begin(); it != (*itproj)["URLs"].end(); ++it)
                proj->urls.push_back(wxString::FromUTF8(std::string(*it)));
            proj->tags.clear();
            for (nlohmann::json::iterator it = (*itproj)["Tags"].begin(); it != (*itproj)["Tags"].end(); ++it)
                proj->tags.push_back(wxString::FromUTF8(std::string(*it)));
            proj->category = cat_elem;
            proj->treeid = NULL;
            projects.push_back(proj);
        }

        // Correct the indices of the projects, to make sure they're in proper order
        if (projects.size() > 0)
        {
            index = 0;
            std::sort(projects.begin(), projects.end(), &project_sorter);
            for (Project* proj : projects)
                proj->index = index++;

            // Now add the project to the tree
            for (Project* proj : projects)
            {
                proj->treeid = tree->AppendItem(cat_id, proj->displayname);
                cat_elem->pages.push_back(proj);
            }
        }
    }
}


/*==============================
    json_save
    Load a list of projects from a JSON file and populate the
    project tree
    @param  The working directory of the program
    @param  A pointer to the category list
==============================*/

void json_save(wxString workingdir, std::vector<Category*>* categorylist_projects)
{
    wxTextFile out_proj(workingdir + wxString("/roms.json"));
    nlohmann::json projectjson = {};
    projectjson["Categories"] = {};

    // Handle projects
    for (Category* cat : *categorylist_projects)
    {
        std::string catstr = cat->foldername.ToStdString();
        projectjson["Categories"][catstr] = {};
        projectjson["Categories"][catstr]["Index"] = cat->index;
        projectjson["Categories"][catstr]["DisplayName"] = cat->displayname.ToUTF8();
        projectjson["Categories"][catstr]["Description"] = cat->description.ToUTF8();
        projectjson["Categories"][catstr]["Pages"] = {};
        for (void* child : cat->pages)
        {
            Project* proj = (Project*)child;
            std::string projstr = proj->filename.ToStdString();
            wxTextFile projmd(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
            projectjson["Categories"][catstr]["Pages"][projstr] = {};
            projectjson["Categories"][catstr]["Pages"][projstr]["Index"] = proj->index;
            projectjson["Categories"][catstr]["Pages"][projstr]["DisplayName"] = proj->displayname.ToUTF8();
            projectjson["Categories"][catstr]["Pages"][projstr]["Icon"] = proj->icon.ToUTF8();
            projectjson["Categories"][catstr]["Pages"][projstr]["Date"] = proj->date.ToUTF8();
            projectjson["Categories"][catstr]["Pages"][projstr]["ToolTip"] = proj->tooltip.ToUTF8();
            projectjson["Categories"][catstr]["Pages"][projstr]["Images"] = {};
            for (wxString str :  proj->images)
                projectjson["Categories"][catstr]["Pages"][projstr]["Images"].push_back(str.ToUTF8());
            projectjson["Categories"][catstr]["Pages"][projstr]["URLs"] = {};
            for (wxString str :  proj->urls)
                projectjson["Categories"][catstr]["Pages"][projstr]["URLs"].push_back(str.ToUTF8());
            projectjson["Categories"][catstr]["Pages"][projstr]["Tags"] = {};
            for (wxString str :  proj->tags)
                projectjson["Categories"][catstr]["Pages"][projstr]["Tags"].push_back(str.ToUTF8());
            if (!wxDirExists(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/")))
                wxFileName::Mkdir(workingdir + wxString("/projects/") + cat->foldername + wxString("/markdown/"));
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
    out_proj.AddLine(wxString::FromUTF8(projectjson.dump(4)));
    out_proj.Write();
    out_proj.Close();
}