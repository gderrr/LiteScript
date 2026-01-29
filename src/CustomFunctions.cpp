#include "CustomFunctions.h"

#include <algorithm>
#include <any>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <shared_mutex>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <typeinfo>
#include <unistd.h>
#include <unordered_map>
#include <vector>

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
        else if (i == "thread") ret.push_back(make_unique<Thread>());
        else if (i == "math") ret.push_back(make_unique<Math>());
        else if (i == "unix") ret.push_back(make_unique<Unix>());
        else if (i == "filesystem") ret.push_back(make_unique<Filesystem>());

        else {
            cerr << "Imported module is not a Litescript module: " << i << endl;
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
        // === START DEFINTION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = getpid();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_ppid;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINTION ===

        auto& a = args[0];
        int& ret = any_cast<reference_wrapper<int>&>(a).get();
        ret = getppid();

        // === END DEFINITION ===

        return true;
    }
    else if (function == "waitpid;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

        auto& a = args[0];
        int& x = any_cast<reference_wrapper<int>&>(a).get();
        std::this_thread::sleep_for(std::chrono::milliseconds(x));

        // === END DEFINITION ===

        return true;
    }
    else if (function == "time;") {
        if (args.size() < 1) IncorrectNumArguments();
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
        // === START DEFINTION ===

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
            else if (mode == "w" || mode == "write") open_mode |= ios::out | ios::trunc;
            else if (mode == "r/w" || mode == "read/write") open_mode |= ios::in | ios::out | ios::trunc;
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



        // === END DEFINITION ===

        return true;
    }
    else if (function == "remove;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        

        // === END DEFINITION ===

        return true;
    }
    else if (function == "path_exists;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "is_file;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "is_dir;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "walk;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_cwd;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "set_cwd;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_filename;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    else if (function == "get_extension;") {
        if (args.size() != 2) IncorrectNumArguments();
        // === START DEFINITION ===



        // === END DEFINITION ===

        return true;
    }
    return false;
}