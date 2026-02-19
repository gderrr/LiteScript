#include "CustomFunctions.h"

#include <algorithm>
#include <any>
#include <arpa/inet.h>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <functional>
#include <fstream>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <pqxx/pqxx>
#include <random>
#include <set>
#include <shared_mutex>
#include <signal.h>
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <typeinfo>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "Extras.h"
#include "httplib.h"
#include "json.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

using namespace std;

void IncorrectNumArguments () {
    cerr << "Incorrect number of arguments for imported function." << endl;
    exit(1); 
}

// Helper to elminate boilerplate in argument retrieval
template <typename T> T& ref_get (any& a) {
    return any_cast<reference_wrapper<T>&>(a).get();
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
        else if (i == "thread") ret.push_back(make_unique<Thread>());
        else if (i == "math") ret.push_back(make_unique<Math>());
        else if (i == "unix") ret.push_back(make_unique<Unix>());
        else if (i == "filesystem") ret.push_back(make_unique<Filesystem>());
        else if (i == "network") ret.push_back(make_unique<Network>());
        else if (i == "gui") ret.push_back(make_unique<GUI>());
        else if (i == "database") ret.push_back(make_unique<Database>());

        else {
            cerr << "Imported module is not a LiteScript module: " << i << endl;
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
    else if (function == "cut;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        int& start = any_cast<reference_wrapper<int>&>(a).get();
        int& end = any_cast<reference_wrapper<int>&>(b).get();
        string& str = any_cast<reference_wrapper<string>&>(c).get();
        str.erase(start, end-start+1);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "paste;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& substr = any_cast<reference_wrapper<string>&>(a).get();
        int& idx = any_cast<reference_wrapper<int>&>(b).get();
        string& str = any_cast<reference_wrapper<string>&>(c).get();
        str.insert(idx, substr);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "replace;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& from = any_cast<reference_wrapper<string>&>(a).get();
        string& to = any_cast<reference_wrapper<string>&>(b).get();
        string& str = any_cast<reference_wrapper<string>&>(c).get();
        if (!from.empty()) {
            size_t pos = 0;
            while ((pos = str.find(from, pos)) != string::npos) {
                str.replace(pos, from.length(), to);
                pos += to.length();
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "reverse;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        reverse(str.begin(), str.end());

        // === END DEFINITION ===

        return true;
    }
    else if (function == "split;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& str = any_cast<reference_wrapper<string>&>(a).get();
        map<Key,any>& cont = any_cast<reference_wrapper<map<Key,any>>&>(b).get();
        cont.clear();
        int idx = 0;
        size_t i = 0;
        while (i < str.size()) {
            while (i < str.size() && isspace(static_cast<unsigned char>(str[i]))) i++;
            if (i >= str.size()) break;
            size_t start = i;
            while (i < str.size() && !isspace(static_cast<unsigned char>(str[i]))) i++;
            cont[Key{int(idx)}] = str.substr(start, i - start);
            idx++;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_integer;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        int& dst = any_cast<reference_wrapper<int>&>(b).get();
        dst = stoi(src);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_real;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        dst = stof(src);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "to_string;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& src = any_cast<reference_wrapper<int>&>(a).get();
            dst = to_string(src);
        }
        else if (a.type() == typeid(reference_wrapper<float>)) {
            float& src = any_cast<reference_wrapper<float>&>(a).get();
            dst = to_string(src);
        }
        else if (a.type() == typeid(reference_wrapper<string>)) {
            string& src = any_cast<reference_wrapper<string>&>(a).get();
            dst = src;
        }

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   THREAD
/////////////////////////////////////

thread_local int currentThreadIndex = -1;

Thread::ActiveThreadsGuard::ActiveThreadsGuard(std::atomic<int>& c, int n)
    : counter(c) {
    counter.store(n);
}

Thread::ActiveThreadsGuard::~ActiveThreadsGuard() {
    counter.store(0);
}

bool Thread::execute (const string& function, vector<any>& args) {
    if (function == "parallel;") {
        if (args.size() < 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        int& n = any_cast<reference_wrapper<int>&>(a).get();
        auto& job = any_cast<storedInterpret&>(b);
        ActiveThreadsGuard guard(activeThreads, n);
        vector<thread> threads;
        threads.reserve(n);
        for (int i = 0; i < n; i++) {
            threads.emplace_back([job, i]() mutable {
                currentThreadIndex = i;
                job.runInterpret();
            });
        }
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        currentThreadIndex = -1;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "thread_id;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = currentThreadIndex;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "num_threads;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = activeThreads.load();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "cache_line_bytes;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        // Default to 64 bytes.
        ret = 64;
        #if defined(_SC_LEVEL1_DCACHE_LINESIZE)
        long size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        if (size > 0) ret = static_cast<int>(size);
        #endif

        // === END DEFINITION ===

        return true;
    }
    else if (function == "num_cores;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        long cores = sysconf(_SC_NPROCESSORS_ONLN);
        if (cores < 1) cores = 1;
        ret = static_cast<int>(cores);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "lock_mutex;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        implicitMutexes[idx].lock();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "unlock_mutex;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        implicitMutexes[idx].unlock();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "async_thread;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& job = any_cast<storedInterpret&>(a);
        std::thread([job]() mutable {job.runInterpret();}).detach();

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   MATH
/////////////////////////////////////

constexpr float PI = 3.1415927f;

Math::Math () {
    mt.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

bool Math::execute (const string& function, vector<any>& args) {
    if (function == "sin;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(sin(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(sin(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "cos;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(cos(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(cos(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tan;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(tan(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(tan(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "asin;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(asin(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(asin(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "acos;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(acos(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(acos(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "atan;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(atan(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(atan(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "sinh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(sinh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(sinh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "cosh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(cosh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(cosh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tanh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(tanh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(tanh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "asinh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(asinh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(asinh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "acosh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(acosh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(acosh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "atanh;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(atanh(x));
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(atanh(x));
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "deg2rad;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(x) * (PI / 180.f);
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(x) * (PI / 180.f);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "rad2deg;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        float& dst = any_cast<reference_wrapper<float>&>(b).get();
        if (a.type() == typeid(reference_wrapper<int>)) {
            int& x = any_cast<reference_wrapper<int>&>(a).get();
            dst = static_cast<float>(x) * (180.f / PI);
        }
        else {
            float& x = any_cast<reference_wrapper<float>&>(a).get();
            dst = static_cast<float>(x) * (180.f / PI);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "rand_integer;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        int& min = any_cast<reference_wrapper<int>&>(a).get();
        int& max = any_cast<reference_wrapper<int>&>(b).get();
        int& dst = any_cast<reference_wrapper<int>&>(c).get();
        std::uniform_int_distribution<int> dist(min, max);
        dst = dist(mt);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "rand_real;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        float& min = any_cast<reference_wrapper<float>&>(a).get();
        float& max = any_cast<reference_wrapper<float>&>(b).get();
        float& dst = any_cast<reference_wrapper<float>&>(c).get();
        std::uniform_real_distribution<float> dist(min, max);
        dst = dist(mt);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_pi;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        float& dst = any_cast<reference_wrapper<float>&>(a).get();
        dst = PI;

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   UNIX
/////////////////////////////////////

Unix::Unix () {
    for (int i = 0; i < implicitPipes.size(); i++) {
        // -1 indicates unused pipe.
        implicitPipes[i].first = implicitPipes[i].second = -1;
    }
}

Unix::~Unix () {
    for (int i = 0; i < implicitPipes.size(); i++) {
        // Close pipes automatically at destruction 
        if (implicitPipes[i].first != -1) close(implicitPipes[i].first);
        if (implicitPipes[i].second != -1) close(implicitPipes[i].second);
    }
}

bool Unix::execute (const string& function, vector<any>& args) {
    if (function == "fork;") {
        if (args.size() > 1) IncorrectNumArguments();
        // === START DEFINITION ===

        int ret = fork();
        if (args.size() == 1) {
            auto& a = args[0];
            int& dst = any_cast<reference_wrapper<int>&>(a).get();
            dst = ret;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "exec;") {
        if (args.size() < 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& file = any_cast<reference_wrapper<string>&>(a).get();
        vector<char*> argVec;
        for (int i = 1; i < args.size(); i++) {
            auto& x = args[i];
            string& arg = any_cast<reference_wrapper<string>&>(x).get();
            argVec.push_back(const_cast<char*>(arg.c_str()));
        }
        argVec.push_back(nullptr);
        execv(file.c_str(), argVec.data());
        execvp(file.c_str(), argVec.data()); // Fallback in case execv fails, search with path

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_pid;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = getpid();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_ppid;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = getppid();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "waitpid;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& pid = any_cast<reference_wrapper<int>&>(a).get();
        int ret = waitpid(pid, nullptr, 0);
        if (args.size() == 2) {
            auto& b = args[1];
            int& dst = any_cast<reference_wrapper<int>&>(b).get();
            dst = ret;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "kill;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        int& pid = any_cast<reference_wrapper<int>&>(a).get();
        int& sig = any_cast<reference_wrapper<int>&>(b).get();
        kill(pid, sig);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "sleep;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& x = any_cast<reference_wrapper<int>&>(a).get();
        std::this_thread::sleep_for(std::chrono::milliseconds(x));

        // === END DEFINITION ===

        return true;
    }
    else if (function == "time;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINITION ===

        uint64_t t = static_cast<uint64_t>(time(nullptr));
        uint32_t low  = static_cast<uint32_t>(t & 0xFFFFFFFFULL);
        uint32_t high = static_cast<uint32_t>(t >> 32);
        auto& a = args[0];
        int& dst1 = any_cast<reference_wrapper<int>&>(a).get();
        dst1 = static_cast<int>(low);
        if (args.size() == 2) {
            auto& b = args[1];
            int& dst2 = any_cast<reference_wrapper<int>&>(b).get();
            dst2 = static_cast<int>(high);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "heap_allocate;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& bytes = any_cast<reference_wrapper<int>&>(b).get();
        void *ptr = malloc(bytes);
        auto it = memoryChunks.find(id);
        if (it != memoryChunks.end()) {
            // If it already exists, free and reallocate with new chunk
            it->second.reset(ptr);
        }
        else {
            memoryChunks.try_emplace(id, ptr, free);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "heap_read;") {
        if (args.size() < 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& offset = any_cast<reference_wrapper<int>&>(b).get();
        auto it = memoryChunks.find(id);
        if (it != memoryChunks.end()) {
            if (c.type() == typeid(reference_wrapper<int>)) {
                int& dst = any_cast<reference_wrapper<int>&>(c).get();
                int value;
                memcpy(&value, static_cast<char*>(it->second.get())+offset, sizeof(int));
                dst = value;
            }
            else if (c.type() == typeid(reference_wrapper<float>)) {
                float& dst = any_cast<reference_wrapper<float>&>(c).get();
                float value;
                memcpy(&value, static_cast<char*>(it->second.get())+offset, sizeof(float));
                dst = value;
            }
            else if (c.type() == typeid(reference_wrapper<string>)) {
                if (args.size() != 4) IncorrectNumArguments();
                auto& d = args[3];
                string& dst = any_cast<reference_wrapper<string>&>(c).get();
                int& bytes = any_cast<reference_wrapper<int>&>(d).get();
                const char* value = static_cast<char*>(it->second.get()) + offset;
                dst = string(value, bytes);
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "heap_write;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& offset = any_cast<reference_wrapper<int>&>(b).get();
        auto it = memoryChunks.find(id);
        if (it != memoryChunks.end()) {
            if (c.type() == typeid(reference_wrapper<int>)) {
                int& src = any_cast<reference_wrapper<int>&>(c).get();
                int value = src;
                memcpy(static_cast<char*>(it->second.get())+offset, &value, sizeof(int));
            }
            else if (c.type() == typeid(reference_wrapper<float>)) {
                float& src = any_cast<reference_wrapper<float>&>(c).get();
                float value = src;
                memcpy(static_cast<char*>(it->second.get())+offset, &value, sizeof(float));
            }
            else if (c.type() == typeid(reference_wrapper<string>)) {
                string& src = any_cast<reference_wrapper<string>&>(c).get();
                // Potentally expensive copy, might rewrite this later
                string value = src;
                memcpy(static_cast<char*>(it->second.get())+offset, src.data(), src.size());
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "heap_free;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        auto it = memoryChunks.find(id);
        if (it != memoryChunks.end()) {
            memoryChunks.erase(it);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "pipe_open;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        int fd[2];
        if (pipe(fd) == -1) {
            cerr << "Pipe error." << endl;
            exit(1);
        }
        implicitPipes[idx].first = fd[0];
        implicitPipes[idx].second = fd[1];

        // === END DEFINITION ===

        return true;
    }
    else if (function == "pipe_read;") {
        if (args.size() < 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        if (b.type() == typeid(reference_wrapper<int>)) {
            int& dst = any_cast<reference_wrapper<int>&>(b).get();
            int aux;
            int n = read(implicitPipes[idx].first, &aux, sizeof(int));
            dst = aux;
        }
        else if (b.type() == typeid(reference_wrapper<float>)) {
            float& dst = any_cast<reference_wrapper<float>&>(b).get();
            float aux;
            int n = read(implicitPipes[idx].first, &aux, sizeof(float));
            dst = aux;
        }
        else if (b.type() == typeid(reference_wrapper<string>)) {
            if (args.size() != 3) IncorrectNumArguments();
            auto& c = args[2];
            string& dst = any_cast<reference_wrapper<string>&>(b).get();
            int& bytes = any_cast<reference_wrapper<int>&>(c).get();
            dst.clear();
            dst.resize(bytes);
            int n = read(implicitPipes[idx].first, &dst[0], bytes);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "pipe_write;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        if (b.type() == typeid(reference_wrapper<int>)) {
            int& src = any_cast<reference_wrapper<int>&>(b).get();
            int aux = src;
            int n = write(implicitPipes[idx].second, &aux, sizeof(int));
        }
        else if (b.type() == typeid(reference_wrapper<float>)) {
            float& src = any_cast<reference_wrapper<float>&>(b).get();
            float aux = src;
            int n = write(implicitPipes[idx].second, &aux, sizeof(float));
        }
        else if (b.type() == typeid(reference_wrapper<string>)) {
            string& src = any_cast<reference_wrapper<string>&>(b).get();
            int n = write(implicitPipes[idx].second, src.data(), src.size());
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "pipe_close_read;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        if (implicitPipes[idx].first != -1) {
            close(implicitPipes[idx].first);
            implicitPipes[idx].first = -1;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "pipe_close_write;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        if (implicitPipes[idx].second != -1) {
            close(implicitPipes[idx].second);
            implicitPipes[idx].second = -1;
        }

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   FILESYSTEM
/////////////////////////////////////

thread_local unordered_map<string, unique_ptr<fstream>> Filesystem::fileStreams;

bool Filesystem::fileStreamExists (const string& id) {
    return fileStreams.find(id) != fileStreams.end();
}

bool Filesystem::execute (const string& function, vector<any>& args) {
    if (function == "file_open;") {
        if (args.size() < 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& path = any_cast<reference_wrapper<string>&>(b).get();
        ios_base::openmode open_mode = ios::binary;
        if (args.size() == 3) {
            auto& c = args[2];
            string& mode = any_cast<reference_wrapper<string>&>(c).get();
            if (mode == "r" || mode == "read") open_mode |= ios::in;
            else if (mode == "w" || mode == "write") open_mode |= ios::out;
            else if (mode == "r/w" || mode == "read/write") open_mode |= ios::in | ios::out;
            else {
                cerr << "Unknown mode for opening file." << endl;
                exit(1);
            }
        } else {
            open_mode |= ios::in | ios::out | ios::trunc;
        }
        if (!filesystem::exists(path)) {
            std::ofstream(path).close();
        }
        auto stream = make_unique<fstream>(path, open_mode);
        if (!stream->is_open()) {
            cerr << "Unexpected error while opening file." << endl;
            exit(1);
        }
        fileStreams[id] = move(stream);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_close;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        fileStreams.erase(id);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_read;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& bytes = any_cast<reference_wrapper<int>&>(b).get();
        string& dst = any_cast<reference_wrapper<string>&>(c).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            if (fs.good() && !fs.eof()) {
                dst = string(bytes, '\0');
                fs.read(dst.data(), bytes);
                dst.resize(fs.gcount());
                auto pos = fs.tellg();
                if (pos != -1) {
                    fs.seekp(pos);
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_readline;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            if (fs.good() && !fs.eof()) {
                dst.clear();
                getline(fs, dst);
                auto pos = fs.tellg();
                if (pos != -1) {
                    fs.seekp(pos);
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_readall;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        if (fileStreamExists(id)) {
            // Read entire file while maintaining cursor where it was
            auto& fs = *fileStreams[id];
            streampos aux = fs.tellg();
            fs.seekg(0, ios::end);
            size_t sz = fs.tellg();
            fs.seekg(0);
            dst = string(sz, '\0');
            fs.read(dst.data(), sz);
            fs.seekg(aux);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_write;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& src = any_cast<reference_wrapper<string>&>(b).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            if (fs.good() && !fs.eof()) {
                fs << src;
                fs.flush();
                auto pos = fs.tellp();
                if (pos != -1) {
                    fs.seekg(pos);
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_cursor;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& dst = any_cast<reference_wrapper<int>&>(b).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            dst = static_cast<int>(fs.tellg());
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_move_cursor;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& x = any_cast<reference_wrapper<int>&>(b).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            auto pos = fs.tellg();
            if (pos != -1) {
                fs.seekp(pos+static_cast<streampos>(x));
                fs.seekg(pos+static_cast<streampos>(x));
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "file_set_cursor;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& x = any_cast<reference_wrapper<int>&>(b).get();
        if (fileStreamExists(id)) {
            auto& fs = *fileStreams[id];
            if (x < 0) {
                fs.seekg(0, std::ios::end);
                fs.seekp(0, std::ios::end);
            } else {
                fs.seekg(x, std::ios::beg);
                fs.seekp(x, std::ios::beg);
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "dir_create;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& name = any_cast<reference_wrapper<string>&>(a).get();
        filesystem::path pth = filesystem::current_path();
        if (args.size() == 2) {
            auto& b = args[1];
            string& str = any_cast<reference_wrapper<string>&>(b).get();
            pth = filesystem::path(str);
            if (!pth.is_absolute()) {
                pth = filesystem::current_path() / pth;
            }
        }
        filesystem::path target = pth / name;
        filesystem::create_directories(target);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "remove;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        filesystem::remove_all(path);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "path_exists;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        int& dst = any_cast<reference_wrapper<int>&>(b).get();
        dst = filesystem::exists(path) ? 1 : 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "is_file;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        int& dst = any_cast<reference_wrapper<int>&>(b).get();
        dst = filesystem::is_regular_file(path) ? 1 : 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "is_dir;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        int& dst = any_cast<reference_wrapper<int>&>(b).get();
        dst = filesystem::is_directory(path) ? 1 : 0;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "walk;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        map<Key,any>& cont = any_cast<reference_wrapper<map<Key,any>>&>(b).get();
        cont.clear();
        int idx = 0;
        for (const auto& entry : filesystem::directory_iterator(path)) {
            cont[Key{int(idx)}] = entry.path().filename().string();
            idx++;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_cwd;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& dst = any_cast<reference_wrapper<string>&>(a).get();
        dst = filesystem::current_path().string();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_cwd;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        if (filesystem::exists(src)) filesystem::current_path(src);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_filename;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        dst = filesystem::path(path).filename().string();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_extension;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& path = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        if (filesystem::is_regular_file(path)) dst = filesystem::path(path).extension().string();
        else dst = "";

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   NETWORK
/////////////////////////////////////

using json = nlohmann::json;
thread_local map<string, unique_ptr<Network::HttpServer>> Network::httpServers;
thread_local map<string, unique_ptr<Network::TcpSocket, Network::TcpSocketDeleter>> Network::tcpSockets;
static const string b64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void Network::TcpSocketDeleter::operator() (TcpSocket* sock) const noexcept {
    if (sock) {
        if (sock->fd >= 0) {
            ::close(sock->fd);
        }
        delete sock;
    }
}

map<Key,any> buildRequestContainer (const httplib::Request req) {
    map<Key,any> ret;
    ret[Key{string("method")}] = req.method;
    ret[Key{string("path")}] = req.path;
    ret[Key{string("version")}] = req.version;
    ret[Key{string("body")}] = req.body;
    map<Key,any> hdrs;
    for (auto& [k, v]: req.headers) {
        hdrs[Key{string(k)}] = v;
    }
    ret[Key{string("headers")}] = hdrs;
    map<Key,any> prms;
    for (auto& [k, v]: req.params) {
        prms[Key{string(k)}] = v;
    }
    ret[Key{string("params")}] = prms;
    return ret;
}

string base64_encode (const string& input) {
    string out;
    int val = 0, valb = -6;
    for (uint8_t c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

string base64_decode (const string& input) {
    vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[b64_chars[i]] = i;
    string out;
    int val = 0, valb = -8;
    for (uint8_t c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

any json_to_any (const json& j) {
    if (j.is_object()) {
        map<Key, any> obj;
        for (auto& [k, v] : j.items()) {
            obj[Key{string(k)}] = json_to_any(v);
        }
        return obj;
    }
    else if (j.is_array()) {
        map<Key, any> arr;
        int i = 0;
        for (auto& v : j) {
            arr[Key{int(i++)}] = json_to_any(v);
        }
        return arr;
    }
    else if (j.is_string()) return j.get<string>();
    else if (j.is_number_integer()) return j.get<int>();
    else if (j.is_number_unsigned()) return static_cast<int>(j.get<unsigned int>());
    else if (j.is_number_float()) return static_cast<float>(j.get<double>());
    else if (j.is_boolean()) return j.get<bool>() ? string("true") : string("false");
    else if (j.is_null()) return string("null");
    else return j.dump();
}

json any_to_json (const any& a) {
    if (a.type() == typeid(map<Key, any>)) {
        map<Key, any> cont = any_cast<map<Key, any>>(a);
        bool allInt = true;
        vector<pair<int, const any*>> indexed;
        for (const auto& [k, v] : cont) {
            if (holds_alternative<int>(k.value)) {
                indexed.push_back({get<int>(k.value), &v});
            } else {
                allInt = false;
                break;
            }
        }
        if (allInt) {
            json arr = json::array();
            for (auto& [idx, val] : indexed) {
                arr.push_back(any_to_json(*val));
            }
            return arr;
        }
        json obj = json::object();
        for (auto& [k, v] : cont) {
            if (holds_alternative<string>(k.value)) {
                obj[get<string>(k.value)] = any_to_json(v);
            }
            else if (holds_alternative<int>(k.value)) {
                obj[to_string(get<int>(k.value))] = any_to_json(v);
            }
            else if (holds_alternative<float>(k.value)) {
                obj[to_string(get<float>(k.value))] = any_to_json(v);
            }
            else {
                obj["?"] = any_to_json(v);
            }
        }
        return obj;
    }
    else if (a.type() == typeid(string)) {
        string val = any_cast<string>(a);
        if (val == "true") return true;
        if (val == "false") return false;
        if (val == "null") return nullptr;
        return val;
    }
    else if (a.type() == typeid(int)) return any_cast<int>(a);
    else if (a.type() == typeid(float)) return any_cast<float>(a);
    else return nullptr;
}

bool Network::execute (const string& function, vector<any>& args) {
    if (function == "http_listen;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        int& port = any_cast<reference_wrapper<int>&>(b).get();
        auto s = make_unique<HttpServer>();
        s->port = port;
        httpServers[id] = move(s);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_route;") {
        if (args.size() != 4) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        auto& d = args[3];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& method = any_cast<reference_wrapper<string>&>(b).get();
        string& path = any_cast<reference_wrapper<string>&>(c).get();
        auto& handler = any_cast<storedInterpret&>(d);
        auto it = httpServers.find(id);
        if (it != httpServers.end()) {
            it->second->routes[method + " " + path] = handler;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_start;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        auto it = httpServers.find(id);
        if (it != httpServers.end()) {
            auto& srv = it->second;
            for (auto& [key, handler] : srv->routes) {
                auto space = key.find(' ');
                string method = key.substr(0, space);
                string path = key.substr(space + 1);

                auto callback = [handler](const httplib::Request& req, httplib::Response& res) {
                    storedInterpret copy = handler;
                    map<Key, any> reqCont = buildRequestContainer(req);
                    copy.args = { reqCont };
                    any result = copy.runInterpret();

                    int status = 200;
                    string body;
                    string content_type = "text/plain";
                    if (result.has_value()) {
                        // In the case where we are returning a simple string, handle it as a base case
                        if (result.type() == typeid(string)) body = any_cast<string>(result);
                        // Otherwise, only accept LTS containers with a valid response structure
                        else {
                            map<Key, any> resCont = any_cast<map<Key,any>>(result);
                            status = any_cast<int>(resCont.at(Key{string("status")}));
                            content_type = any_cast<string>(resCont.at(Key{string("content_type")}));
                            body = any_cast<string>(resCont.at(Key{string("body")}));
                        }
                        res.status = status;
                        res.set_content(body, content_type);
                    }
                };

                if (method == "GET") srv->server.Get(path, callback);
                else if (method == "POST") srv->server.Post(path, callback);
                else if (method == "PUT") srv->server.Put(path, callback);
                else if (method == "DELETE") srv->server.Delete(path, callback);
            }

            srv->thread = thread([srv = srv.get()]() {
                srv->server.listen("0.0.0.0", srv->port);
            });
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_get;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& url = any_cast<reference_wrapper<string>&>(a).get();
        string& path = any_cast<reference_wrapper<string>&>(b).get();
        string& dst = any_cast<reference_wrapper<string>&>(c).get();
        httplib::Client cli(url.c_str());
        if (auto res = cli.Get(path.c_str())) dst = res->body;
        else dst.clear();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_post;") {
        if (args.size() < 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& url = any_cast<reference_wrapper<string>&>(a).get();
        string& path = any_cast<reference_wrapper<string>&>(b).get();
        string& body = any_cast<reference_wrapper<string>&>(c).get();
        string contentType = "text/plain";
        if (args.size() == 4) {
            auto& d = args[3];
            string& ct = any_cast<reference_wrapper<string>&>(d).get();
            contentType = ct;
        }
        httplib::Client cli(url.c_str());
        auto res = cli.Post(path.c_str(), body, contentType);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_put;") {
        if (args.size() < 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& url = any_cast<reference_wrapper<string>&>(a).get();
        string& path = any_cast<reference_wrapper<string>&>(b).get();
        string& body = any_cast<reference_wrapper<string>&>(c).get();
        string contentType = "text/plain";
        if (args.size() == 4) {
            auto& d = args[3];
            string& ct = any_cast<reference_wrapper<string>&>(d).get();
            contentType = ct;
        }
        httplib::Client cli(url.c_str());
        auto res = cli.Put(path.c_str(), body, contentType);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "http_delete;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& url = any_cast<reference_wrapper<string>&>(a).get();
        string& path = any_cast<reference_wrapper<string>&>(b).get();
        httplib::Client cli(url.c_str());
        auto res = cli.Delete(path.c_str());

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_listen;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& host = any_cast<reference_wrapper<string>&>(b).get();
        int& port = any_cast<reference_wrapper<int>&>(c).get();
        int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            cerr << "tcp_listen: Socket error." << endl;
            exit(1);
        }
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (host.empty() || host == "0.0.0.0") {
            addr.sin_addr.s_addr = INADDR_ANY;
        }
        else {
            inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
        }
        if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            cerr << "tcp_listen: Bind error." << endl;
            ::close(fd);
            exit(1);
        }
        if (listen(fd, SOMAXCONN) < 0) {
            cerr << "tcp_listen: Listen error." << endl;
            ::close(fd);
            exit(1);
        }
        unique_ptr<TcpSocket, TcpSocketDeleter> sock(
            new TcpSocket(),
            TcpSocketDeleter{}
        );
        sock->fd = fd;
        sock->isListener = true;
        sock->host = host;
        sock->port = port;
        tcpSockets[id] = move(sock);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_accept;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& conn_id = any_cast<reference_wrapper<string>&>(b).get();
        auto it = tcpSockets.find(id);
        if (it != tcpSockets.end()) {
            int listen_fd = it->second->fd;
            sockaddr_in clientAddr{};
            socklen_t len = sizeof(clientAddr);
            int client_fd = ::accept(listen_fd, (sockaddr*)&clientAddr, &len);
            if (client_fd >= 0) {
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
                int client_port = ntohs(clientAddr.sin_port);

                unique_ptr<TcpSocket, TcpSocketDeleter> conn(
                    new TcpSocket(),
                    TcpSocketDeleter{}
                );
                conn->fd = client_fd;
                conn->isListener = false;
                conn->host = ipstr;
                conn->port = client_port;
                tcpSockets[conn_id] = move(conn);
            }
        }
 
        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_connect;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& conn_id = any_cast<reference_wrapper<string>&>(a).get();
        string& host = any_cast<reference_wrapper<string>&>(b).get();
        int& port = any_cast<reference_wrapper<int>&>(c).get(); 
        int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            cerr << "tcp_connect: Socket error." << endl;
            exit(1);
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
        if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(fd);
        } else {
            unique_ptr<TcpSocket, TcpSocketDeleter> conn(
                new TcpSocket(),
                TcpSocketDeleter{}
            );
            conn->fd = fd;
            conn->isListener = false;
            conn->host = host;
            conn->port = port;
            tcpSockets[conn_id] = move(conn);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_send;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& data = any_cast<reference_wrapper<string>&>(b).get();
        auto it = tcpSockets.find(id);
        if (it != tcpSockets.end()) {
            TcpSocket* sock = it->second.get();
            if (sock && sock->fd != -1) {
                size_t totalSent = 0;
                while (totalSent < data.size()) {
                    ssize_t sent = ::send(sock->fd, data.data() + totalSent, data.size() - totalSent, 0);
                    if (sent <= 0) {
                        // Assume error or closed connection, to return true to signal end of execution.
                        return true;
                    }
                    totalSent += sent;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_recv;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        auto it = tcpSockets.find(id);
        if (it != tcpSockets.end()) {
            TcpSocket* sock = it->second.get();
            if (sock && sock->fd != -1) {
                dst.clear();
                char buf[4096];
                while (true) {
                    ssize_t n = ::recv(sock->fd, buf, sizeof(buf), 0);
                    if (n == 0) break;
                    if (n < 0) {
                        dst.clear();
                        break;
                    }
                    dst.append(buf, n);
                    if (n < sizeof(buf)) break;
                }
            } 
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "tcp_close;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& id = any_cast<reference_wrapper<string>&>(a).get();
        auto it = tcpSockets.find(id);
        if (it != tcpSockets.end()) tcpSockets.erase(it);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "udp_send;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& ip = any_cast<reference_wrapper<string>&>(a).get();
        int& port = any_cast<reference_wrapper<int>&>(b).get();
        string& data = any_cast<reference_wrapper<string>&>(c).get();
        int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (sock >= 0) {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
            ::sendto(sock, data.c_str(), data.size(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            ::close(sock);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "udp_recv;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        auto& c = args[2];
        string& ip = any_cast<reference_wrapper<string>&>(a).get();
        int& port = any_cast<reference_wrapper<int>&>(b).get();
        string& out = any_cast<reference_wrapper<string>&>(c).get();
        int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (sock >= 0) {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;
            if (::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
                ::close(sock);
                return true;
            }
            char buf[4096];
            sockaddr_in sender{};
            socklen_t len = sizeof(sender);
            ssize_t n = ::recvfrom(sock, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&sender), &len);
            if (n > 0) out.assign(buf, n);
            else out.clear();
            ::close(sock);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "dns_resolve;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& hostname = any_cast<reference_wrapper<string>&>(a).get();
        string& result = any_cast<reference_wrapper<string>&>(b).get();
        addrinfo hints{}, *res = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (::getaddrinfo(hostname.c_str(), nullptr, &hints, &res) == 0) {
            sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(res->ai_addr);
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            result = ip;
            ::freeaddrinfo(res);
        }
        else {
            result.clear();
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "local_ip;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        string& result = any_cast<reference_wrapper<string>&>(a).get();
        int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            result = "0.0.0.0";
            return true;
        }

        sockaddr_in tmp{};
        tmp.sin_family = AF_INET;
        tmp.sin_port = htons(80);
        inet_pton(AF_INET, "8.8.8.8", &tmp.sin_addr);
        if (::connect(sock, reinterpret_cast<sockaddr*>(&tmp), sizeof(tmp)) == 0) {
            sockaddr_in name{};
            socklen_t len = sizeof(name);
            ::getsockname(sock, reinterpret_cast<sockaddr*>(&name), &len);
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &name.sin_addr, ip, sizeof(ip));
            result = ip;
        }
        else {
            result = "0.0.0.0";
        }
        ::close(sock);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "encode_base64;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        dst = base64_encode(src);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "decode_base64;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        dst = base64_decode(src);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "json_to_container;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        string& src = any_cast<reference_wrapper<string>&>(a).get();
        map<Key,any>& dst = any_cast<reference_wrapper<map<Key,any>>&>(b).get();
        dst.clear();
        json j = json::parse(src, nullptr, false);
        if (!j.is_object()) return true;
        for (auto& [k, v] : j.items()) {
            dst[Key{string(k)}] = json_to_any(v);
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "container_to_json;") {
        if (args.size() < 2) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        auto& b = args[1];
        map<Key,any>& src = any_cast<reference_wrapper<map<Key,any>>&>(a).get();
        string& dst = any_cast<reference_wrapper<string>&>(b).get();
        json j = any_to_json(src);
        if (args.size() == 3) {
            auto& c = args[2];
            int& n = any_cast<reference_wrapper<int>&>(c).get();
            dst = j.dump(n);
        }
        else {
            dst = j.dump();
        }

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   GUI
/////////////////////////////////////

const float REFERENCE_FONT_SIZE = 9.0f; // Empirically determined constant to align with Google Docs Standards

GUI::GUI () {
    // === GLFW Init ===
    if (!glfwInit()) {
        cerr << "gui: Failed to initialize GLFW" << endl;
        exit(1);
    }
}

GUI::~GUI() {
    // Optional: make sure no other thread is touching GLFW/ImGui here.

    // If you track initialization per-window, prefer that flag.
    for (auto it = windows.begin(); it != windows.end();) {
        Window &win = it->second;

        // If both are nullptr, skip and erase to avoid future accidental use
        if (!win.glfwWindow && !win.imguiCtx) {
            it = windows.erase(it);
            continue;
        }

        // If we still have a valid GLFW window, make its context current.
        if (win.glfwWindow) {
            // Safety: if making current fails nothing else should be called
            glfwMakeContextCurrent(win.glfwWindow);
        }

        // If we have an ImGui context associated with this window, set it current.
        if (win.imguiCtx) {
            ImGui::SetCurrentContext(win.imguiCtx);

            // Only call backend shutdown if you previously initialized them for this context.
            // If you don't track that, call them guardedly (they expect a valid context).
            // Wrap in try/catch to avoid exceptions propagating (C backends shouldn't throw).
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();

            ImGui::DestroyContext(win.imguiCtx);
            win.imguiCtx = nullptr;

            // After destroying an ImGui context, make sure no ImGui context is left current.
            ImGui::SetCurrentContext(nullptr);
        }

        // Destroy GLFW window if it exists
        if (win.glfwWindow) {
            glfwDestroyWindow(win.glfwWindow);
            win.glfwWindow = nullptr;
        }

        // erase the entry so later code cannot touch it
        it = windows.erase(it);
    }

    // Finally ensure no context is current and terminate GLFW once.
    glfwMakeContextCurrent(nullptr);
    glfwTerminate();
}

bool GUI::execute (const string& function, vector<any>& args) {
    if (function == "make_window;") {
        if (args.size() != 5) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        int& x = ref_get<int>(args[1]);
        int& y = ref_get<int>(args[2]);
        int& w = ref_get<int>(args[3]);
        int& h = ref_get<int>(args[4]);
        
        Window win;
        win.title = title;
        win.pos = ImVec2((float)x, (float)y);
        win.size = ImVec2((float)w, (float)h);
        win.visible = false;
        windows[title] = win;

        // === END DEFINITION ===
        return true;
    }
    else if (function == "add_button;") {
        if (args.size() != 7) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        int& h = ref_get<int>(args[5]);
        storedInterpret& func = any_cast<storedInterpret&>(args[6]);

        Widget wdg;
        wdg.type = Widget::BUTTON;
        wdg.id = label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, (float)h);
        wdg.callback = func;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===
        return true;
    }
    else if (function == "add_label;") {
        if (args.size() < 5) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& fontSize = ref_get<int>(args[4]);
        string text = (args.size() == 6) ? ref_get<string>(args[5]) : "";

        Widget wdg;
        wdg.type = Widget::LABEL;
        wdg.id = label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)fontSize, 0.0f);
        wdg.text = text;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_textfield;") {
        if (args.size() < 6) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        int& h = ref_get<int>(args[5]);
        string text = (args.size() == 7) ? ref_get<string>(args[6]) : "Enter text...";

        Widget wdg;
        wdg.type = Widget::TEXTFIELD;
        wdg.id = "##" + label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, (float)h);
        wdg.text = text;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_checkbox;") {
        if (args.size() != 6) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        int& h = ref_get<int>(args[5]);

        Widget wdg;
        wdg.type = Widget::CHECKBOX;
        wdg.id = "##" + label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, (float)h);
        wdg.bvalue = false;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_slider;") {
        if (args.size() < 7) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        int& min = ref_get<int>(args[5]);
        int& max = ref_get<int>(args[6]);
        int def = (args.size() == 8) ? ref_get<int>(args[7]) : min;

        Widget wdg;
        wdg.type = Widget::SLIDER;
        wdg.id = "##" + label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, 0.0f);
        wdg.max = max;
        wdg.min = min;
        wdg.ivalue = def;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_progress_bar;") {
        if (args.size() < 6) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        int& h = ref_get<int>(args[5]);
        float def = (args.size() == 7) ? ref_get<float>(args[6]) : 0.0f;

        Widget wdg;
        wdg.type = Widget::PROGRESS;
        wdg.id = label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, (float)h);
        wdg.fvalue = def;
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_dropdown;") {
        if (args.size() != 6) IncorrectNumArguments();
        // === START DEFINITION ===

        string& winTitle = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& x = ref_get<int>(args[2]);
        int& y = ref_get<int>(args[3]);
        int& w = ref_get<int>(args[4]);
        map<Key,any>& opt = ref_get<map<Key,any>>(args[5]);

        Widget wdg;
        wdg.type = Widget::DROPDOWN;
        wdg.id = "##" + label;
        wdg.pos = ImVec2((float)x, (float)y);
        wdg.size = ImVec2((float)w, 0.0f);
        wdg.options.clear();
        for (auto& [k, v] : opt) {
            wdg.options.push_back(any_cast<string>(v)); // Change it to a set later for ordering
        }
        wdg.text = wdg.options.empty() ? "" : wdg.options[0];
        windows[winTitle].widgets.push_back(wdg);

        // === END DEFINITION ===

        return true;
    }
    else if (function == "add_window;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        auto& win = windows[title];
        if (!win.glfwWindow) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            win.glfwWindow = glfwCreateWindow((int)win.size.x, (int)win.size.y, win.title.c_str(), nullptr, nullptr);
            if (!win.glfwWindow) {
                cerr << "gui: Failed to create GLFW window" << endl;
                exit(1);
            }
            glfwSetWindowPos(win.glfwWindow, (int)win.pos.x, (int)win.pos.y);
            glfwMakeContextCurrent(win.glfwWindow);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                cerr << "gui: Failed to initialize GLAD." << endl;
                exit(1);
            }
            win.imguiCtx = ImGui::CreateContext();
            ImGui::SetCurrentContext(win.imguiCtx);
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            ImGui_ImplGlfw_InitForOpenGL(win.glfwWindow, true);
            ImGui_ImplOpenGL3_Init("#version 330");
        }
        else {
            glfwShowWindow(win.glfwWindow);
            glfwMakeContextCurrent(win.glfwWindow);
            ImGui::SetCurrentContext(win.imguiCtx);
        }
        win.visible = true;
        win.running = true;

        // === END DEFINITION ===

        return true;
    }
    else if (function == "close_window;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        auto it = windows.find(title);
        if (it != windows.end()) {
            Window& win = it->second;
            win.closing = true;
            win.visible = false;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "visible_window;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        int& dst = ref_get<int>(args[1]);
        auto it = windows.find(title);
        if (it != windows.end()) {
            dst = it->second.visible;
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_label;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        string& str = ref_get<string>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == label && w.type == Widget::LABEL) {
                    w.text = str;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_textfield;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        string& str = ref_get<string>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::TEXTFIELD) {
                    str = w.text;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_textfield;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        string& str = ref_get<string>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::TEXTFIELD) {
                    w.text = str;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_checkbox;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& n = ref_get<int>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::CHECKBOX) {
                    if (w.bvalue) n = 1;
                    else n = 0;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_checkbox;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::CHECKBOX) {
                    w.bvalue = true;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "clear_checkbox;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::CHECKBOX) {
                    w.bvalue = false;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "toggle_checkbox;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::CHECKBOX) {
                    w.bvalue = !w.bvalue;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_slider;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& n = ref_get<int>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::SLIDER) {
                    n = w.ivalue;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_slider;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        int& n = ref_get<int>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::SLIDER) {
                    w.ivalue = n;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_progress_bar;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        float& x = ref_get<float>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == label && w.type == Widget::PROGRESS) {
                    w.fvalue = x;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_dropdown;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        string& str = ref_get<string>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::DROPDOWN) {
                    str = w.text;
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_dropdown;") {
        if (args.size() != 3) IncorrectNumArguments();
        // === START DEFINITION ===

        string& title = ref_get<string>(args[0]);
        string& label = ref_get<string>(args[1]);
        string& str = ref_get<string>(args[2]);
        auto it = windows.find(title);
        if (it != windows.end() && it->second.running) {
            for (auto& w : it->second.widgets) {
                if (w.id == "##"+label && w.type == Widget::DROPDOWN) {
                    if (std::find(w.options.begin(), w.options.end(), str) != w.options.end())
                        w.text = str;
                    else
                        w.text.clear();
                    return true;
                }
            }
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "render_gui;") {
        
        // === START DEFINITION ===

        bool anyRunning = true;
        while (anyRunning) {
            glfwPollEvents();
            anyRunning = false;
            for (auto& [_, win] : windows) {
                if (win.closing) continue;
                if (!win.visible || !win.glfwWindow) continue;
                if (glfwWindowShouldClose(win.glfwWindow)) {
                    win.running = false;
                    win.visible = false;
                    win.closing = true;
                    continue;
                }
                anyRunning = anyRunning || win.running;
                glfwMakeContextCurrent(win.glfwWindow);
                ImGui::SetCurrentContext(win.imguiCtx);
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                int dw, dh;
                glfwGetFramebufferSize(win.glfwWindow, &dw, &dh);
                ImGui::SetNextWindowPos(ImVec2(0,0));
                ImGui::SetNextWindowSize(ImVec2((float)dw, (float)dh));
                ImGui::Begin("##root", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | 
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | 
                ImGuiWindowFlags_NoDecoration);
                ImGui::SetWindowFontScale(12 / REFERENCE_FONT_SIZE);
                for (auto& w : win.widgets) {
                    switch (w.type) {
                        case Widget::LABEL:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            ImGui::SetWindowFontScale(w.size.x / REFERENCE_FONT_SIZE);
                            ImGui::Text("%s", w.text.c_str());
                            ImGui::SetWindowFontScale(12 / REFERENCE_FONT_SIZE);
                            break;
                        case Widget::BUTTON:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            if (ImGui::Button(w.id.c_str(), w.size))
                                w.callback.runInterpret();
                            break;
                        case Widget::TEXTFIELD: {
                            size_t buf_size = max<size_t>(w.text.size() + 1, 1024);
                            vector<char> buffer(buf_size);
                            memcpy(buffer.data(), w.text.c_str(), w.text.size() + 1);
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            if (ImGui::InputTextMultiline(w.id.c_str(), buffer.data(), buffer.size(), ImVec2(w.size.x, w.size.y)))
                                w.text = string(buffer.data());
                            break;
                        }
                        case Widget::CHECKBOX:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            ImGui::Checkbox(w.id.c_str(), &w.bvalue);
                            break;
                        case Widget::SLIDER:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            ImGui::SetNextItemWidth(w.size.x);
                            ImGui::SliderInt(w.id.c_str(), &w.ivalue, w.min, w.max);
                            break;
                        case Widget::PROGRESS:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));  
                            ImGui::ProgressBar(w.fvalue, w.size);
                            break;
                        case Widget::DROPDOWN:
                            ImGui::SetCursorPos(ImVec2(w.pos.x, w.pos.y));
                            ImGui::SetNextItemWidth(w.size.x);
                            if (ImGui::BeginCombo(w.id.c_str(), w.text.c_str())) {
                                for (auto& opt : w.options) {
                                    bool selected = (w.text == opt);
                                    if (ImGui::Selectable(opt.c_str(), selected))
                                        w.text = opt;
                                    if (selected) ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                            break;
                        default:
                            break;
                    }
                }
                ImGui::End();
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(win.glfwWindow, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(win.glfwWindow);
            }
        }

        for (auto it = windows.begin(); it != windows.end(); ) {
            Window& win = it->second;
            if (win.closing) {
                if (win.glfwWindow && win.imguiCtx) {
                    ImGui::SetCurrentContext(win.imguiCtx);
                    ImGui_ImplOpenGL3_Shutdown();
                    ImGui_ImplGlfw_Shutdown();
                    ImGui::DestroyContext(win.imguiCtx);
                    win.imguiCtx = nullptr;

                    glfwDestroyWindow(win.glfwWindow);
                    win.glfwWindow = nullptr;
                }
                it = windows.erase(it);
            } else {
                ++it;
            }
        }

        // === END DEFINITION ===

        return true;
    }
    return false;
}

/////////////////////////////////////
//   DATABASE
/////////////////////////////////////

thread_local unique_ptr<Database::DBWrapper> Database::backend = nullptr;
thread_local bool Database::inTx = false;

Database::SQLite::SQLite (const string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "database: Failed to open SQLite database, " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
}

void Database::SQLite::close () {
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void Database::SQLite::begin_transaction () {
    if (!db) {
        cerr << "database: No open database." << endl;
        exit(1);
    }
    char* err = nullptr;
    if (sqlite3_exec(db, "BEGIN_TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "database: SQLite begin failed." << endl;
        exit(1);
    }
}

void Database::SQLite::commit_transaction () {
    if (!db) {
        cerr << "database: No open database." << endl;
        exit(1);
    }
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "database: SQLite commit failed." << endl;
        exit(1);
    }
}

void Database::SQLite::rollback_transaction () {
    if (!db) {
        cerr << "database: No open database." << endl;
        exit(1);
    }
    if (sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        cerr << "database: SQLite rollback failed." << endl;
        exit(1);
    }
}

void Database::SQLite::prepare (const string& sql) {
    if (!db) {
        cerr << "database: No open database." << endl;
        exit(1);
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "database: SQLite prepare failed." << endl;
        exit(1);
    }
}

void Database::SQLite::bind (int index, const std::any& value) {
    if (!stmt) {
        cerr << "database: No prepared statement." << endl;
        exit(1);
    }
    if (index < 1 || index > sqlite3_bind_parameter_count(stmt)) {
        cerr << "database: Invalid bind index." << endl;
        exit(1);
    }
    int rc = SQLITE_OK;
    if (value.type() == typeid(int)) {
        rc = sqlite3_bind_int(stmt, index, any_cast<int>(value));
    }
    else if (value.type() == typeid(float)) {
        rc = sqlite3_bind_double(stmt, index, static_cast<double>(any_cast<float>(value)));
    }
    else {
        const string& s = any_cast<string>(value);
        rc = sqlite3_bind_text(stmt, index, s.c_str(), static_cast<int>(s.size()), SQLITE_TRANSIENT);
    }
    if (rc != SQLITE_OK) {
        cerr << "database: Failed to bind parameter " << index << '.' << endl;
        exit(1);
    } 
}

map<Key,any> Database::SQLite::execute () {
    if (!stmt) {
        cerr << "database: No prepared statement." << endl;
        exit(1);
    }
    map<Key,any> results;
    int rc;
    int row = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        map<Key,any> resultRow;
        for (int i = 0; i < cols; i++) {
            const char* colName = sqlite3_column_name(stmt, i);
            string columnName = colName ? colName : to_string(i);
            int type = sqlite3_column_type(stmt, i);
            switch (type) {
                case SQLITE_INTEGER:
                    resultRow[Key{columnName}] = sqlite3_column_int(stmt, i);
                    break;
                case SQLITE_FLOAT:
                    resultRow[Key{columnName}] = (float)sqlite3_column_double(stmt, i);
                    break;
                case SQLITE_TEXT:
                    resultRow[Key{columnName}] = string((const char*)sqlite3_column_text(stmt, i));
                    break;
                default:
                    resultRow[Key{columnName}] = string("NULL");
            }
        }
        results[Key{row}] = resultRow;
    }
    if (rc != SQLITE_DONE) {
        lastErr = sqlite3_errmsg(db);
    }
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return results;
}