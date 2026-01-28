#pragma once

#include <any>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Extras.h"

// Polymorphism impl

// (Java)                           (C++)
// Function f = new Display();      unique_ptr<Function> f = make_unique<Display>();
// f.execute(...);                  f->execute(...);

class Function {

    public:
    
    virtual bool execute (const std::string& function, std::vector<std::any>& args) = 0;
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
    std::vector<std::unique_ptr<Function>> createFunctions (const std::set<std::string>& imports);

};

class IO: public Function {

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;

};

class Ascii: public Function {

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
};

class Thread: public Function {

    private:

    std::atomic<int> activeThreads;
    std::vector<std::mutex> implicitMutexes{128};
    struct ActiveThreadsGuard {
        std::atomic<int>& counter;
        ActiveThreadsGuard(std::atomic<int>& c, int n);
        ~ActiveThreadsGuard();
    };

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
};

class Math: public Function {

    private:

    std::mt19937 mt;

    public:

    Math();
    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
};

class Unix: public Function {

    private:

    std::map<std::string,void*> memoryChunks;
    std::vector<int> implicitPipes{128};

    public:

    Unix();
    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
    ~Unix();
};