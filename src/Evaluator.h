#pragma once
#include <string>
#include <any>
#include <map>

std::any evaluate (const std::map<std::string,std::any>& variables, const std::string& expression);