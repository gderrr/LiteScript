#include "CustomFunctions.h"
#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include <functional>

FunctionFactory::FunctionFactory () {}

FunctionFactory& FunctionFactory::getInstance () {
    static FunctionFactory INSTANCE;
    return INSTANCE;
}

vector<unique_ptr<Function>> FunctionFactory::createFunctions (const set<string>& imports) {
    vector<unique_ptr<Function>> ret;
    for (string i : imports) {

        // Imported function types go here
        if (i == "io") ret.push_back(make_unique<IO>());

        else {
            cerr << "Imported function is not a Litescript module: " << i << endl;
            exit(1);
        }
    }
    return ret;
}

bool IO::execute (const string& function, vector<any>& args) {

    if (function == "display;") {

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
            else if (a.type() == typeid(storedInterpret)) {
                auto& call = any_cast<storedInterpret&>(a);
                any value = call.runInterpret();
                if (value.type() == typeid(int)) cout << any_cast<int>(value);
                else if (value.type() == typeid(float)) cout << any_cast<float>(value);
                else if (value.type() == typeid(string)) cout << any_cast<string>(value);
                else {
                    cerr << "Unsupported return type of passed function call: " << value.type().name() << endl;
                    exit(1);
                } 
            }
            else {
                cerr << "Unsupported type: " << a.type().name() << endl;
                exit(1);
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "input;") {

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