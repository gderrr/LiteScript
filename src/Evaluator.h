#pragma once

#include <any>
#include <map>
#include <string>

std::any evaluate (const std::map<std::string,std::any>& variables, const std::string& expression);