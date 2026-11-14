#pragma once

#include <string>
#include "page_rom.h"
#include "page_author.h"

std::string string_fromfile(std::string path);
std::string sanitize_tagname(std::string tagname);
std::string sanitize_html(std::string text);
void strreplace_all(std::string& str, const std::string& from, const std::string& to);

std::string date_tostring(time_t date);
bool rom_sorter_date(Page_ROM* lhs, Page_ROM* rhs);
bool taggedpage_sorter(Page* lhs, Page* rhs);
bool author_sorter(Page_Author* lhs, Page_Author* rhs);

void purge_htmlfiles(const std::string& directory);
void makedir(const std::string& directory);