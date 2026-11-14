#pragma once

#include <string>
#include <vector>
#include <memory>
#include "category.h"
#include "page_author.h"
#include "include/json.hpp"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(std::string jsonpath, std::vector<Category*>* categorylist);
void json_loadroms(std::string jsonpath, std::vector<Category*>* categorylist, const std::vector<Page_Author*>* authors, std::vector<std::unique_ptr<Page_Author>>* unknownauthors);
void json_loadauthors(std::string jsonpath, std::vector<Page_Author*>* authors);