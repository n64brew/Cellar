#pragma once

#include <vector>
#include "main.h"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(wxString filepath, std::vector<Category*>* categorylist);
void json_loadroms(wxString workingdir, std::vector<Category*>* categorylist, wxTreeCtrl* tree);
void json_save(wxString workingdir, std::vector<Category*>* categorylist_roms);