#pragma once
#include <vector>
#include <string>
#include <any>
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

class IO: public Function {

    public:

    virtual bool execute (const string& function, vector<any>& args) override;

};