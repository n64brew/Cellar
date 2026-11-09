#pragma once

#include <string>
#include <vector>
#include "category.h"
#include "include/json.hpp"


/*********************************
        Function Prototypes
*********************************/

int json_loadcategories(std::string jsonpath, std::vector<Category*>* categorylist);
void json_loadroms(std::string jsonpath, std::vector<Category*>* categorylist);