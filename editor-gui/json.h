#pragma once

#include <vector>
#include "main.h"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(wxString filepath, std::vector<Category*>* categorylist);
void json_loadauthors(wxString workingdir, std::vector<Author*>* authors);
void json_loadroms(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree);
bool json_save(wxString workingdir, std::vector<Category*>* categorylist_roms, std::vector<Author*>* authors);