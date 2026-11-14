/***************************************************************
                           helper.cpp

This file contains a bunch of helper functions for transforming,
sorting, and finding data types
***************************************************************/

#ifdef _WIN32
    typedef struct IUnknown IUnknown;
    #include <windows.h>
    #include <direct.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif
#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include "helper.h"
#include "markdown.h"


/*==============================
    string_fromfile
    Opens a text file and exports it as a string
    @param  The path to the file to open
    @return The file contents as a string, or ""
==============================*/

std::string string_fromfile(std::string path)
{
    std::string str;
    std::string line;
    bool first = true;

    // Ensure we managed to check and open the file
    std::ifstream file(path);
    if (!file.is_open())
        return "";

    // Read the file into the string, and return it
    while (std::getline(file, line))
    {
        if (!first)
        {
            str += "\r\n";
        }
        str += line;
        first = false;
    }
    return str;
}


/*==============================
    sanitize_tagname
    Sanitizes a tag's name so it's a valid file name
    @param  The tag name to sanitize
    @return The sanitized tag name
==============================*/

std::string sanitize_tagname(std::string tagname)
{
    strreplace_all(tagname, "#", "s");
    strreplace_all(tagname, "+", "p");
    strreplace_all(tagname, ":", "");
    strreplace_all(tagname, "-", "");
    strreplace_all(tagname, "/", "");
    strreplace_all(tagname, ".", "");
    strreplace_all(tagname, "[", "");
    strreplace_all(tagname, "]", "");
    strreplace_all(tagname, "<", "");
    strreplace_all(tagname, ">", "");
    strreplace_all(tagname, "\"", "");
    strreplace_all(tagname, "\\", "");
    strreplace_all(tagname, "?", "");
    strreplace_all(tagname, " ", "");
    return tagname;
}


/*==============================
    sanitize_html
    Sanitizes HTML text
    @param  The text to sanitize
    @return The sanitized text
==============================*/

std::string sanitize_html(std::string text)
{
    strreplace_all(text, "&", "&amp;");
    strreplace_all(text, "<", "&lt;");
    strreplace_all(text, ">", "&gt;");
    strreplace_all(text, "\"", "&quot;");
    strreplace_all(text, "'", "&#39;");
    return text;
}


/*==============================
    author_imagepath
    Gets an author's image path relative to the authors folder
    @param  The stored image path
    @return The relative image path
==============================*/

std::string author_imagepath(std::string image)
{
    if (image.compare(0, 8, "authors/") == 0)
        image.erase(0, 8);
    return image;
}


/*==============================
    strreplace_all
    Replace all ocurrences of a substring
    Taken from https://stackoverflow.com/a/3418285
    @param The string to replace
    @param The substring we want to replace
    @param What we want to replace it with
==============================*/

void strreplace_all(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;

    if (from.empty())
        return;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


/*==============================
    date_tostring
    Convert a timestamp to date
    @param  The input date
    @return The date string
==============================*/

std::string date_tostring(time_t date)
{
    std::tm utc{};
    std::ostringstream oss;
    #ifdef _WIN32
        gmtime_s(&utc, &date);
    #else
        gmtime_r(&date, &utc);
    #endif
    oss << std::put_time(&utc, "%B %d, %Y");
    return oss.str();
}


/*==============================
    rom_sorter_date
    Sorts two rom pages by their date
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool rom_sorter_date(Page_ROM* lhs, Page_ROM* rhs)
{
    return lhs->date > rhs->date;
}


/*==============================
    taggedpage_sorter
    Sorts two tagged pages by their name
    @param  The first element
    @param  The second element
    @return Whether the first element should be ordered before
            the second
==============================*/

bool taggedpage_sorter(Page* lhs, Page* rhs)
{
    std::string lhs_name, rhs_name;
    if (dynamic_cast<Page_ROM*>(lhs))
        lhs_name = dynamic_cast<Page_ROM*>(lhs)->displayname;
    else 
        lhs_name = "";
    if (dynamic_cast<Page_ROM*>(rhs))
        rhs_name = dynamic_cast<Page_ROM*>(rhs)->displayname;
    else
        rhs_name = "";
    return lhs_name < rhs_name;
}


/*==============================
    author_sorter
    Sorts authors by their display names
    @param  The first author
    @param  The second author
    @return Whether the first author should be ordered first
==============================*/

bool author_sorter(Page_Author* lhs, Page_Author* rhs)
{
    return std::lexicographical_compare(
        lhs->displayname.begin(), lhs->displayname.end(),
        rhs->displayname.begin(), rhs->displayname.end(),
        [](unsigned char lhs_char, unsigned char rhs_char)
        {
            return std::tolower(lhs_char) < std::tolower(rhs_char);
        });
}


/*==============================
    purge_htmlfiles
    Deletes all HTML files in a directory
    @param The directory to purge
==============================*/

void purge_htmlfiles(const std::string& directory)
{
    #ifdef _WIN32
        std::string search = directory + "\\*.html";
        WIN32_FIND_DATAA finddata;
        HANDLE findhandle = FindFirstFileA(search.c_str(), &finddata);
        if (findhandle == INVALID_HANDLE_VALUE)
            return;
        do
        {
            if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::string filepath = directory + "\\" + finddata.cFileName;
                DeleteFileA(filepath.c_str());
            }

        }
        while (FindNextFileA(findhandle, &finddata));
        FindClose(findhandle);
    #else
        struct dirent* entry;
        DIR* dir = opendir(directory.c_str());
        if (!dir)
            return;
        while ((entry = readdir(dir)) != NULL)
        {
            std::string filename = entry->d_name;
            if (filename.size() >= 5 && filename.compare(filename.size() - 5, 5, ".html") == 0)
            {
                std::string filepath = directory + "/" + filename;
                std::remove(filepath.c_str());
            }
        }
        closedir(dir);
    #endif
}


/*==============================
    makedir
    Makes a directory. 
    Does not make them recursively!
    @param The directory to create
==============================*/

void makedir(const std::string& directory)
{
    #ifdef _WIN32
        _mkdir(directory.c_str());
    #else
        mkdir(directory.c_str(), 0755);
    #endif
}