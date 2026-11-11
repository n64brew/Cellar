#pragma once

#include <string>
#include "page_rom.h"

std::string string_fromfile(std::string path);
std::string sanitize_tagname(std::string tagname);
void strreplace_all(std::string& str, const std::string& from, const std::string& to);

bool date_fromstring(const std::string& input, std::tm* output);
bool rom_sorter_date(Page_ROM* lhs, Page_ROM* rhs);
bool taggedpage_sorter(Page* lhs, Page* rhs);

void purge_htmlfiles(const std::string& directory);
void makedir(const std::string& directory);