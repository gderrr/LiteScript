#pragma once
#include <vector>
#include <string>
#include <set>
#include <any>
#include <memory>
#include "Extras.h"
using namespace std;

// Polymorphism impl

// (Java)                           (C++)
// Function f = new Display();      unique_ptr<Function> f = make_unique<Display>();
// f.execute(...);                  f->execute(...);

class Function {

    public:
    
    virtual bool execute (const string& function, vector<any>& args) = 0;
    virtual ~Function () {};

};

// Fancy singleton + simple factory design pattern
class FunctionFactory {
    
    private:

    FunctionFactory();
    FunctionFactory(const FunctionFactory&) = delete;
    FunctionFactory& operator= (const FunctionFactory&) = delete;

    public:

    static FunctionFactory& getInstance();
    vector<unique_ptr<Function>> createFunctions (const set<string>& imports);

};

class IO: public Function {

    public:

    virtual bool execute (const string& function, vector<any>& args) override;

};

class Ascii: public Function {

    public:

    virtual bool execute (const string& function, vector<any>& args) override;
};