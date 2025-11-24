#pragma once
#include <string>
#include <any>
#include <map>
using namespace std;

any evaluate (const map<string,any>& variables, const string& expression);