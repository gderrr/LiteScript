#include "CustomFunctions.h"
#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include <functional>

bool IO::execute (const string& function, vector<any>& args) {

    if (function == "display") {

        // === START DEFINITION ===

        for (auto& a: args) {
            if (a.type() == typeid(reference_wrapper<int>)) {
                int& p = any_cast<reference_wrapper<int>&>(a).get();
                cout << p;
            }
            else if (a.type() == typeid(reference_wrapper<float>)) {
                float& p = any_cast<reference_wrapper<float>&>(a).get();
                cout << p;
            }
            else if (a.type() == typeid(reference_wrapper<string>)) {
                string& p = any_cast<reference_wrapper<string>&>(a).get();
                cout << p;
            }
            else {
                cerr << "Unsupported type: " << a.type().name() << endl;
                exit(1);
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "input") {

        // === START DEFINITION ===

        for (auto& a: args) {
            if (a.type() == typeid(reference_wrapper<int>)) {
                int& p = any_cast<reference_wrapper<int>&>(a).get();
                cin >> p;
            }
            else if (a.type() == typeid(reference_wrapper<float>)) {
                float& p = any_cast<reference_wrapper<float>&>(a).get();
                cin >> p;
            }
            else if (a.type() == typeid(reference_wrapper<string>)) {
                string& p = any_cast<reference_wrapper<string>&>(a).get();
                cin >> p;
            }
            else {
                cerr << "Unsupported type: " << a.type().name() << endl;
                exit(1);
            }
        }

        // === END DEFINITION ===

        return true;
    }
    return false;

}