#pragma once
#include <map>
#include <any>
#include <string>
#include <vector>
#include <memory>
#include <any>
#include <variant>
#include "CustomFunctions.h"

// Header for globalVariables so they can be used anywhere
extern std::map<std::string, std::any> globalVariables;

using InterpretFunc = std::any(*)(int, const std::vector<std::any>&);

struct storedInterpret {
    InterpretFunc func;
    int startLine;
    std::vector<std::any> args;

    std::any runInterpret () const;
};

struct Key {
    std::variant<int, float, std::string> value;
    bool operator< (const Key& other) const;
};