#include "CustomFunctions.h"
#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include <functional>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

void IncorrectNumArguments () {
    cerr << "Incorrect number of arguments for imported function." << endl;
    exit(1); 
}

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
        else if (i == "ascii") ret.push_back(make_unique<Ascii>());

        else {
            cerr << "Imported function is not a Litescript module: " << i << endl;
            exit(1);
        }
    }
    return ret;
}

/////////////////////////////////////
//   IO
/////////////////////////////////////

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

/////////////////////////////////////
//   ASCII
/////////////////////////////////////

bool Ascii::execute (const string& function, vector<any>& args) {
    if (function == "len;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        int& num = any_cast<reference_wrapper<int>&>(b).get();
        num = str.size();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "substr;") {
        if (args.size() != 4) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        auto& d = args[3];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        int& start = any_cast<reference_wrapper<int>&>(b).get();
        int& end = any_cast<reference_wrapper<int>&>(c).get();
        string& dst = any_cast<reference_wrapper<string>&>(d).get();
        dst = src.substr(start, end-start+1);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "contains;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        string& substr = any_cast<reference_wrapper<string>&>(b).get();
        int& ret = any_cast<reference_wrapper<int>&>(c).get();
        if (str.find(substr) != string::npos) ret = 1;
        else ret = 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "starts_with;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        string& substr = any_cast<reference_wrapper<string>&>(b).get();
        int& ret = any_cast<reference_wrapper<int>&>(c).get();
        if (str.size() >= substr.size() && str.compare(0, substr.size(), substr) == 0) ret = 1;
        else ret = 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "ends_with;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        string& substr = any_cast<reference_wrapper<string>&>(b).get();
        int& ret = any_cast<reference_wrapper<int>&>(c).get();
        if (str.size() >= substr.size() && str.compare(str.size() - substr.size(), substr.size(), substr) == 0) ret = 1;
        else ret = 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_upper;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        for (int i = 0; i < str.size(); i++) {
            char c = str[i];
            if (c >= 'a' && c <= 'z') {
                str[i] = c - ('a' - 'A');
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_lower;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        for (int i = 0; i < str.size(); i++) {
            char c = str[i];
            if (c >= 'A' && c <= 'Z') {
                str[i] = c + ('a' - 'A');
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "trim;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        auto it = find_if(str.begin(), str.end(), [](unsigned char c) {
            bool isNotSpace;
            if (!isspace(c)) isNotSpace = true;
            else isNotSpace = false;
            return isNotSpace;
        });
        str.erase(str.begin(), it);
        auto it2 = find_if(str.rbegin(), str.rend(), [](unsigned char c) {
            bool isNotSpace;
            if (!isspace(c)) isNotSpace = true;
            else isNotSpace = false;
            return isNotSpace;
        });
        str.erase(it2.base(), str.end());


        // === END DEFINITION ===

        return true;
    }
    else if (function == "trim_left;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        auto it = find_if(str.begin(), str.end(), [](unsigned char c) {
            bool isNotSpace;
            if (!isspace(c)) isNotSpace = true;
            else isNotSpace = false;
            return isNotSpace;
        });
        str.erase(str.begin(), it);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "trim_right;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        auto it = find_if(str.rbegin(), str.rend(), [](unsigned char c) {
            bool isNotSpace;
            if (!isspace(c)) isNotSpace = true;
            else isNotSpace = false;
            return isNotSpace;
        });
        str.erase(it.base(), str.end());

        // === END DEFINITION ===

        return true;
    }
    else if (function == "replace;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "reverse;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "split;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_integer;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_real;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_string;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    return false;
}