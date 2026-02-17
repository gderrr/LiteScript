#pragma once

#include <any>
#include <atomic>
#include <fstream>
#include "glad/glad.h" // Messes up order, i know; but necessary
#include <GLFW/glfw3.h>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "Extras.h"
#include "httplib.h"
#include "imgui/imgui.h"

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

class Filesystem: public Function {

    private:

    static thread_local std::unordered_map<std::string, std::unique_ptr<std::fstream>> fileStreams;

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
    bool fileStreamExists (const std::string& id);
};

class Network: public Function {

    private:

    struct HttpServer {
        httplib::Server server;
        std::map<std::string, storedInterpret> routes;
        std::thread thread;
        int port;
    };
    struct TcpSocket {
        int fd = -1;
        bool isListener = false;
        std::string host;
        int port = 0;
    };
    struct TcpSocketDeleter {
        void operator() (TcpSocket* sock) const noexcept;
    };
    static thread_local std::map<std::string, std::unique_ptr<HttpServer>> httpServers;
    static thread_local std::map<std::string, std::unique_ptr<TcpSocket, TcpSocketDeleter>> tcpSockets;

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
};

class GUI: public Function {

    private:

    struct Widget {
        enum Type {
            BUTTON, LABEL, TEXTFIELD, CHECKBOX, SLIDER, PROGRESS, DROPDOWN, IMAGE
        } type;
        std::string id;
        ImVec2 pos, size;
        std::string text;
        int min = 0, max = 100;
        float fvalue = 0.0f;
        int ivalue = 0;
        bool bvalue = false;
        std::vector<std::string> options;
        storedInterpret callback;
    };
    struct Window {
        std::string title;
        ImVec2 pos, size;
        bool visible = false;
        bool running = true;
        bool closing = false;
        GLFWwindow* glfwWindow = nullptr;
        ImGuiContext* imguiCtx = nullptr;
        std::vector<Widget> widgets;
    };
    std::unordered_map<std::string, Window> windows;

    public:

    GUI();
    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
    ~GUI();
};

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
// thread_local TYPE X::x;
// ...