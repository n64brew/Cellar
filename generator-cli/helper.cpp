#include <fstream>
#include <filesystem>
#include <string>
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
    if (!std::filesystem::exists(path))
        return "";
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
    strreplace_all(tagname, " ", "");
    return tagname;
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
    date_fromstring
    Retrieves a date from a string
    @param  The input string
    @param  A pointer to the date to output
    @return Whether the conversion was successful
==============================*/

bool date_fromstring(const std::string& input, std::tm* output)
{
    std::istringstream ss(input);
    ss >> std::get_time(output, "%Y-%m-%d");
    return !ss.fail();
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
    std::tm date_lhs{}, date_rhs{};

    if (!date_fromstring(lhs->date, &date_lhs))
        return false;

    if (!date_fromstring(rhs->date, &date_rhs))
        return true;

    if (date_lhs.tm_year != date_rhs.tm_year)
        return date_lhs.tm_year > date_rhs.tm_year;

    if (date_lhs.tm_mon != date_rhs.tm_mon)
        return date_lhs.tm_mon > date_rhs.tm_mon;

    return date_lhs.tm_mday > date_rhs.tm_mday;
}