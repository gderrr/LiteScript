#pragma once

#include <any>
#include <atomic>
#include <cstring>
#include <fstream>
#include "glad/glad.h" // Messes up order, i know; but necessary
#include <GLFW/glfw3.h>
#include <map>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <random>
#include <set>
#include <sqlite3.h>
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

class Database: public Function {

    private:

    class DBWrapper {

        public:

        virtual void close() = 0;
        virtual void begin_transaction() = 0;
        virtual void commit_transaction() = 0;
        virtual void rollback_transaction() = 0;
        virtual void prepare(const std::string& sql) = 0;
        virtual void bind(int index, const std::any& value) = 0;
        virtual std::map<Key, std::any> execute() = 0;
        virtual std::string last_error() const = 0;
        virtual ~DBWrapper() = default;
    };
    class SQLite: public DBWrapper {

        private:

        sqlite3* db = nullptr;
        sqlite3_stmt* stmt = nullptr;
        std::string lastErr;

        public:

        explicit SQLite (const std::string& path);
        virtual void close() override;
        virtual void begin_transaction() override;
        virtual void commit_transaction() override;
        virtual void rollback_transaction() override;
        virtual void prepare(const std::string& sql) override;
        virtual void bind(int index, const std::any& value) override;
        virtual std::map<Key, std::any> execute() override;
        virtual std::string last_error() const override { return lastErr; };
        ~SQLite() override { close(); };
    };
    class PostgreSQL: public DBWrapper {

        private:

        std::unique_ptr<pqxx::connection> conn;
        std::unique_ptr<pqxx::work> tx;
        std::string preparedSQL;
        std::vector<std::string> params;
        std::string lastErr;

        public:

        explicit PostgreSQL (const std::string& url, const std::string& user, const std::string& pass, const std::string& db);
        virtual void close() override;
        virtual void begin_transaction() override;
        virtual void commit_transaction() override;
        virtual void rollback_transaction() override;
        virtual void prepare(const std::string& sql) override;
        virtual void bind(int index, const std::any& value) override;
        virtual std::map<Key, std::any> execute() override;
        virtual std::string last_error() const override { return lastErr; };
        ~PostgreSQL() override { close(); };
    };
    static thread_local std::unique_ptr<DBWrapper> backend;
    static thread_local bool inTx;

    public:

    virtual bool execute (const std::string& function, std::vector<std::any>& args) override;
    ~Database();
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