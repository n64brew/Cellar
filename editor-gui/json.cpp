/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include <algorithm>
#include "json.h"
#include "helper.h"
#include "include/json.hpp"
#include <wx/file.h>
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
        wxFile file;
        wxString contents;
        wxCharBuffer utf8;
        if (!wxFileExists(filepath))
        {
            wxTextFile newfile;
            wxMessageBox(wxString("JSON file at '"+filepath+"' does not exist. It will be created."), wxString("Error opening JSON"), wxICON_ERROR);
            if (!newfile.Create(filepath))
            {
                wxMessageBox("Could not create the JSON file.", "Error opening JSON", wxICON_ERROR);
                return NULL;
            }
            newfile.AddLine("{}");
            if (!newfile.Write(wxTextFileType_None, wxConvUTF8))
            {
                wxMessageBox("Could not write the JSON file.", "Error opening JSON", wxICON_ERROR);
                return NULL;
            }
            newfile.Close();
        }
        file.Open(filepath);
        if (!file.IsOpened() || !file.ReadAll(&contents, wxConvUTF8))
        {
            wxMessageBox("Could not read the JSON file as UTF-8.", "Error opening JSON", wxICON_ERROR);
            return NULL;
        }
        utf8 = contents.ToUTF8();
        return nlohmann::json::parse(utf8.data(), utf8.data() + utf8.length());
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
        wxString catfoldername = wxString::FromUTF8(it.key());

        // Create the category object
        cat = new Category();
        cat->foldername = catfoldername;
        cat->index = (*it)["Index"];
        cat->displayname = wxString::FromUTF8(std::string((*it)["DisplayName"]));
        cat->description = wxString::FromUTF8(std::string((*it)["Description"]));
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
            if (wxString::FromUTF8(itcat.key()) == cat->foldername)
            {
                cat_elem = cat;
                cat_id = cat->treeid;
                break;
            }
        }

        // Read the rom data from the JSON
        for (nlohmann::json::iterator itrom = (*itcat)["Pages"].begin(); itrom != (*itcat)["Pages"].end(); ++itrom)
        {
            wxString taglist = wxString("");
            ROM* rom = new ROM();
            rom->index = (*itrom)["Index"];
            rom->filename = wxString::FromUTF8(itrom.key());
            rom->displayname = wxString::FromUTF8(std::string((*itrom)["DisplayName"]));
            rom->icon = wxString::FromUTF8(std::string((*itrom)["Icon"]));
            rom->date = (*itrom)["Date"];
            rom->tooltip = wxString::FromUTF8(std::string((*itrom)["ToolTip"]));
            rom->agerating = wxString::FromUTF8(std::string((*itrom)["AgeRating"]));
            rom->contentwarning = wxString::FromUTF8(std::string((*itrom)["ContentWarning"]));
            rom->aidisclosure = wxString::FromUTF8(std::string((*itrom)["AIDisclosure"]));
            rom->wasmodified = false;
            rom->description = string_fromfile(workingdir + wxString("/roms/") + cat_elem->foldername + wxString("/markdown/") + rom->filename + wxString(".md"));
            rom->images.clear();
            for (nlohmann::json::iterator it = (*itrom)["Images"].begin(); it != (*itrom)["Images"].end(); ++it)
                rom->images.push_back(wxString::FromUTF8(std::string(*it)));
            rom->urls.clear();
            for (nlohmann::json::iterator it = (*itrom)["URLs"].begin(); it != (*itrom)["URLs"].end(); ++it)
                rom->urls.push_back(wxString::FromUTF8(std::string(*it)));
            rom->tags.clear();
            for (nlohmann::json::iterator it = (*itrom)["Tags"].begin(); it != (*itrom)["Tags"].end(); ++it)
                rom->tags.push_back(wxString::FromUTF8(std::string(*it)));
            rom->genres.clear();
            for (nlohmann::json::iterator it = (*itrom)["Genres"].begin(); it != (*itrom)["Genres"].end(); ++it)
                rom->genres.push_back(wxString::FromUTF8(std::string(*it)));
            if (itrom->contains("Authors") && (*itrom)["Authors"].is_array())
                for (const auto& author : (*itrom)["Authors"])
                    rom->authors.push_back(wxString::FromUTF8(author.get<std::string>()));
            rom->category = cat_elem;
            rom->treeid = NULL;
            roms.push_back(rom);
        }

        // Correct the indices of the roms, to make sure they're in proper order
        if (roms.size() > 0)
        {
            index = 0;
            std::sort(roms.begin(), roms.end(), &rom_sorter);
            for (ROM* rom : roms)
                rom->index = index++;

            // Now add the rom to the tree
            for (ROM* rom : roms)
            {
                rom->treeid = tree->AppendItem(cat_id, rom->displayname);
                cat_elem->pages.push_back(rom);
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
    wxTextFile out_rom(workingdir + wxString("/roms/roms.json"));
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
    {
        wxFileName::Mkdir(workingdir + wxString("/authors/markdown"), 0777, wxPATH_MKDIR_FULL);
        create_gitkeep(workingdir + "/authors/markdown");
    }
    for (Author* author : *authors)
    {
        wxTextFile markdown(workingdir + wxString("/authors/markdown") + "/" + author->filename + ".md");
        if (!(markdown.Exists() ? markdown.Open(wxConvUTF8) : markdown.Create()))
        {
            wxMessageBox(wxString::Format("Could not open %s.md for saving.", author->filename), "Error saving authors", wxOK|wxICON_ERROR);
            return false;
        }
        markdown.Clear();
        markdown.AddLine(author->description);
        if (!markdown.Write(wxTextFileType_None, wxConvUTF8))
        {
            wxMessageBox(wxString::Format("Could not save %s.md.", author->filename), "Error saving authors", wxOK|wxICON_ERROR);
            return false;
        }
        markdown.Close();
    }

    // Handle roms
    for (Category* cat : *categorylist_roms)
    {
        std::string catstr = cat->foldername.ToUTF8().data();
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
            create_gitkeep(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/"));
            create_gitkeep(workingdir + wxString("/roms/") + cat->foldername + wxString("/images/"));
            create_gitkeep(workingdir + wxString("/roms/") + cat->foldername + wxString("/icons/"));
            create_gitkeep(workingdir + wxString("/roms/") + cat->foldername + wxString("/downloads/"));
        }
        for (void* child : cat->pages)
        {
            ROM* rom = (ROM*)child;
            std::string romstr = rom->filename.ToUTF8().data();
            wxTextFile rommd(workingdir + wxString("/roms/") + cat->foldername + wxString("/markdown/") + rom->filename + wxString(".md"));
            romjson["Categories"][catstr]["Pages"][romstr] = {};
            romjson["Categories"][catstr]["Pages"][romstr]["Index"] = rom->index;
            romjson["Categories"][catstr]["Pages"][romstr]["DisplayName"] = rom->displayname.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["Icon"] = rom->icon.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["Date"] = rom->date;
            romjson["Categories"][catstr]["Pages"][romstr]["ToolTip"] = rom->tooltip.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["AgeRating"] = rom->agerating.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["ContentWarning"] = rom->contentwarning.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["AIDisclosure"] = rom->aidisclosure.ToUTF8();
            romjson["Categories"][catstr]["Pages"][romstr]["Images"] = {};
            for (wxString str :  rom->images)
                romjson["Categories"][catstr]["Pages"][romstr]["Images"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][romstr]["URLs"] = {};
            for (wxString str :  rom->urls)
                romjson["Categories"][catstr]["Pages"][romstr]["URLs"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][romstr]["Tags"] = {};
            for (wxString str :  rom->tags)
                romjson["Categories"][catstr]["Pages"][romstr]["Tags"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][romstr]["Genres"] = {};
            for (wxString str :  rom->genres)
                romjson["Categories"][catstr]["Pages"][romstr]["Genres"].push_back(str.ToUTF8());
            romjson["Categories"][catstr]["Pages"][romstr]["Authors"] = nlohmann::json::array();
            for (const wxString& author : rom->authors)
                romjson["Categories"][catstr]["Pages"][romstr]["Authors"].push_back(author.ToUTF8());
            if (!(rommd.Exists() ? rommd.Open(wxConvUTF8) : rommd.Create()))
            {
                wxMessageBox(wxString::Format("Could not open %s.md for saving.", rom->filename), "Error saving ROMs", wxOK|wxICON_ERROR);
                return false;
            }
            rommd.Clear();
            rommd.AddLine(rom->description);
            if (!rommd.Write(wxTextFileType_None, wxConvUTF8))
            {
                wxMessageBox(wxString::Format("Could not save %s.md.", rom->filename), "Error saving ROMs", wxOK|wxICON_ERROR);
                return false;
            }
            rommd.Close();
        }
    }

    // Dump the JSONs to text files
    if (!(out_rom.Exists() ? out_rom.Open(wxConvUTF8) : out_rom.Create()))
    {
        wxMessageBox("Could not open roms.json for saving.", "Error saving ROMs", wxOK|wxICON_ERROR);
        return false;
    }
    out_rom.Clear();
    out_rom.AddLine(wxString::FromUTF8(romjson.dump(4)));
    if (!out_rom.Write(wxTextFileType_None, wxConvUTF8))
    {
        wxMessageBox("Could not save roms.json.", "Error saving ROMs", wxOK|wxICON_ERROR);
        return false;
    }
    out_rom.Close();
    if (!(out_authors.Exists() ? out_authors.Open(wxConvUTF8) : out_authors.Create()))
    {
        wxMessageBox("Could not open authors.json for saving.", "Error saving authors", wxOK|wxICON_ERROR);
        return false;
    }
    out_authors.Clear();
    out_authors.AddLine(wxString::FromUTF8(authorjson.dump(4)));
    if (!out_authors.Write(wxTextFileType_None, wxConvUTF8))
    {
        wxMessageBox("Could not save authors.json.", "Error saving authors", wxOK|wxICON_ERROR);
        return false;
    }
    out_authors.Close();
    return true;
}