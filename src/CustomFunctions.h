#pragma once

#include <any>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "Extras.h"

// NOTE: given that the DS that holds functions is global, all threads can access it, causing problems to class attributes.
// So, if an attribute can be thread local to be protected, must put "static thread_local" and define it in .cpp file.
//
// .h:
// class X : public Function {
// ...
// private:
// static thread_local TYPE x;
// ...
// };
// 
// .cpp:
// thread_local TYPE x;
// ...
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

    std::map<std::string, std::unique_ptr<void, decltype(&free)>> memoryChunks;
    std::vector<std::pair<int,int>> implicitPipes{128};

    public:

    Unix();
    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
    ~Unix();
};