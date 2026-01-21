#include "Extras.h"
#include <iostream> // DEBUG
using namespace std;

mutex_map<string, any> globalVariables;

any storedInterpret::runInterpret() const {
    return func(startLine, args);
}

bool Key::operator< (const Key& other) const {
    return value < other.value;
}