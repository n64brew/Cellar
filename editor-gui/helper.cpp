/***************************************************************
                           helper.cpp

This file contains a bunch of helper functions for transforming,
sorting, and finding data types
***************************************************************/

#include "helper.h"
#include <wx/textfile.h>
#include <wx/file.h>
#include <wx/msgdlg.h>


/*==============================
    category_sorter
    Sorts two category objects by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool category_sorter(Category* lhs, Category* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    rom_sorter
    Sorts two rom pages by their index
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool rom_sorter(ROM* lhs, ROM* rhs)
{
    return lhs->index < rhs->index;
}


/*==============================
    date_fromstring
    Retrieves a date from a string
    @param  The input string
    @param  A pointer to the date to output
    @return Whether the conversion was successful
==============================*/

bool date_fromstring(wxString input, wxDateTime* output)
{
    wxString::const_iterator end;
    if (!output->ParseDate(input, &end))
        return false;
    return true;
}


/*==============================
    rom_sorter_date
    Sorts two rom pages by their date
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool rom_sorter_date(ROM* lhs, ROM* rhs)
{
    return lhs->date < rhs->date;
}


/*==============================
    taggedpage_sorter
    Sorts two tagged pages by their name
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool taggedpage_sorter(TaggedPage lhs, TaggedPage rhs)
{
    wxString lhs_name, rhs_name;
    lhs_name = ((ROM*)lhs.page)->displayname;
    rhs_name = ((ROM*)rhs.page)->displayname;
    return lhs_name < rhs_name;
}


/*==============================
    treeitem_iscategory
    Checks if the item in the given tree is a category object
    @param  The tree to search
    @param  The item to check
    @return Whether the item is a category object
==============================*/

bool treeitem_iscategory(wxTreeCtrl* tree, wxTreeItemId item)
{
    return tree->GetRootItem() == tree->GetItemParent(item);
}


/*==============================
    string_fromfile
    Opens a text file and exports it as a string
    @param  The path to the file to open
    @return The file contents as a string, or ""
==============================*/

wxString string_fromfile(wxString path)
{
    wxTextFile file;
    wxString str = wxString("");
    size_t linecount;

    // Ensure we managed to open the file
    if (!wxFileExists(path))
        return wxString("");
    if (!file.Open(path, wxConvUTF8))
    {
        wxMessageBox(wxString::Format("Could not read '%s' as UTF-8", path), "Error opening JSON", wxICON_ERROR);
        return wxString("");
    }

    // Read the file into the string, and return it
    linecount = file.GetLineCount();
    if (linecount == 0)
        return str;
    str += file.GetFirstLine();
    while (!file.Eof())
    {
        if (file.GetCurrentLine() < linecount-1)
            str += wxString("\r\n");
        str += file.GetNextLine();
    }
    return str;
}


/*==============================
    sanitize_tagname
    Sanitizes a tag's name so it's a valid file name
    @param  The tag name to sanitize
    @return The sanitized tag name
==============================*/

wxString sanitize_tagname(wxString tagname)
{
    tagname.Trim(true).Trim(false);
    tagname.Replace("#", "s");
    tagname.Replace("+", "p");
    tagname.Replace("'", "");
    tagname.Replace(":", "");
    tagname.Replace("-", "");
    tagname.Replace("/", "");
    tagname.Replace(".", "");
    tagname.Replace("[", "");
    tagname.Replace("]", "");
    tagname.Replace("<", "");
    tagname.Replace(">", "");
    tagname.Replace("\"", "");
    tagname.Replace("\\", "");
    tagname.Replace("?", "");
    tagname.Replace(" ", "");
    return tagname;
}


/*==============================
    md4c_funcptr_handlestr
    A helper function for appending strings during html 
    generation from markdown
    @param  The input markdown text
    @param  The size of the input markdown text
    @param  A pointer to the outputted content
==============================*/

void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output)
{
    ((wxString*)output)->Append(input, inputsize);
}


/*==============================
    matches_filter
    Checks if a text string contains the substring in the
    filter.
    @param  The text we want to check
    @param  The filter to perform
    @return Whether the string matches the filter
==============================*/

static bool matches_filter(const wxString& text, const wxString& filter)
{
    return text.Lower().Contains(filter);
}


/*==============================
    filter_tree_recursive
    Filters a tree recursively, keeping parent nodes visible
    @param  The tree to recurse
    @param  The parent item
    @param  The text we want to use to filter
    @return Whether the string matches the filter
==============================*/

static bool filter_tree_recursive(wxTreeCtrl* tree, const wxTreeItemId& item, const wxString& filter)
{
    bool itemmatch = tree->GetItemText(item).Lower().Contains(filter);
    bool childmatch = false;
    
    wxArrayTreeItemIds children;
    wxTreeItemIdValue cookie;
    wxTreeItemId child = tree->GetFirstChild(item, cookie);

    // Collect children before modifying the tree.
    while (child.IsOk())
    {
        children.Add(child);
        child = tree->GetNextChild(item, cookie);
    }

    // Recursively filter children.
    for (size_t  i=0; i<children.size(); ++i)
        if (filter_tree_recursive(tree, children[i], filter))
            childmatch = true;

    // Remove items that don't match.
    if (!itemmatch && !childmatch)
    {
        tree->Delete(item);
        return false;
    }
    return true;
}


/*==============================
    filter_tree
    Filters a tree, removing elements that don't match the filter
    @param  The tree to recurse
    @param  The text we want to use to filter
==============================*/

void filter_tree(wxTreeCtrl* tree, wxString filter)
{
    wxTreeItemId root = tree->GetRootItem();
    if (!root.IsOk())
        return;
    wxString filter_lower = filter.Lower();
    filter_tree_recursive(tree, root, filter_lower);
}


/*==============================
    create_gitkeep
    Creates a .gitkeep file in the specified directory
    This is to keep the folder structure more beginner
    friendly. The generator then cleans up gitkeeps from
    non-empty folders.
    @param  The directory path
==============================*/

void create_gitkeep(wxString directory)
{
    wxString gitkeep_path = directory + "/.gitkeep";
    wxFile gitkeep(gitkeep_path, wxFile::write_append);
    gitkeep.Close();
}