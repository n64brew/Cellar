#pragma once

typedef struct IUnknown IUnknown; 

#include "main.h"
#include "include/json.hpp"
#include "include/md4c/md4c-html.h"


/*********************************
        Function Prototypes
*********************************/

// Finding functions
bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item);
wxString string_fromfile(wxString path);

// Sorting functions
bool category_sorter(Category* lhs, Category* rhs);
bool rom_sorter(ROM* lhs, ROM* rhs);
bool rom_sorter_date(ROM* lhs, ROM* rhs);
bool taggedpage_sorter(TaggedPage lhs, TaggedPage rhs);

// Date functions
bool date_fromstring(wxString input, wxDateTime* output);

// Markdown helper functions
wxString sanitize_tagname(wxString tagname);
void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output);
wxString* md_sanitize(wxString* input);
wxString* md_unsanitize(wxString* input);