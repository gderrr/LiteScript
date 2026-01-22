#include "CustomFunctions.h"

#include <algorithm>
#include <any>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
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
        std::lock_guard<std::mutex> guard(mutexPool);
        auto it = implicitMutexes.find(idx);
        if (it == implicitMutexes.end()) {
            auto ptr = std::make_unique<std::mutex>();
            std::mutex* m = ptr.get();
            implicitMutexes.emplace(idx, std::move(ptr));
            m->lock();
        } else {
            it->second->lock();
        }

        // === END DEFINITION ===

        return true;
    }
    else if (function == "unlock_mutex;") {
        if (args.size() != 1) IncorrectNumArguments();
        // === START DEFINITION ===

        auto& a = args[0];
        int& idx = any_cast<reference_wrapper<int>&>(a).get();
        std::lock_guard<std::mutex> guard(mutexPool);
        auto it = implicitMutexes.find(idx);
        if (it != implicitMutexes.end()) it->second->unlock();

        // === END DEFINITION ===

        return true;
    }
    return false;
}