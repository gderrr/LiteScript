#pragma once
#include <map>
#include <any>
#include <string>
#include <vector>
#include <memory>
#include <any>
#include "CustomFunctions.h"
using namespace std;

// Header for globalVariables so they can be used anywhere
extern map<string, any> globalVariables;

using InterpretFunc = any(*)(int, const vector<any>&);

struct storedInterpret {
    InterpretFunc func;
    int startLine;
    vector<any> args;

    any runInterpret () const;
};