#pragma once

#include <string>
#include <vector>
#include <memory>
#include "category.h"
#include "page_author.h"
#include "page_rom.h"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(std::string jsonpath, std::vector<Category*>* categorylist);
void json_loadroms(std::string jsonpath, std::vector<Category*>* categorylist, std::vector<Page_Author*>* authors, std::vector<std::unique_ptr<Page_Author>>* unknownauthors);
void json_loadauthors(std::string jsonpath, std::vector<Page_Author*>* authors);
std::string json_romfilter(Page_ROM* page);