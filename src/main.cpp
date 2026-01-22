#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>
#include <cctype>
#include <vector>
#include <cstdlib>
#include <typeinfo>
#include <stack>
#include <map>
#include <set>
#include <functional>
#include <variant>
#include <iterator>
#include <deque>
#include <any>

#include "CustomFunctions.h"
#include "Extras.h"
#include "Evaluator.h"

using namespace std;

//////////////////////////////////////////////////////////////////
// UTILS
//////////////////////////////////////////////////////////////////

struct TLine {
    int indentLevel;
    string code;
};

// Global Variables
map<string,int> funcs;
vector<TLine> program;
set<string> importStatements;
vector<unique_ptr<Function>> importedFunctions;

struct TLoop {
    int line;
    int indentLevel;
};

bool isBlankLine (const string& line) {
    return all_of(line.begin(), line.end(), [](char c) { return isspace(c); } );
}

bool isComment (const string& line) {
    for (char c: line) {
        if (isspace(static_cast<unsigned char>(c))) {
            continue;
        }
        return c == '$';
    }
    return false;
}

vector<string> split (int& tabCount, const string& line) {
    tabCount = 0;
    int i = 0;
    for (; i < line.size() && line[i] == '\t'; i++) tabCount++;

    vector<string> words;
    string current;
    for (; i < line.size(); i++) {
        char c = line[i];
        if (isspace(c)) {
            if (!current.empty()) {
                words.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        words.push_back(current);
    }

    return words;
}

vector<string> readClean (const string& filePath) {
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filePath << endl;
        exit(1);
    }

    vector<string> readFile;
    string line;
    while (getline(file, line)) {
        // Removes lines with no relevant text (including comments)
        if (!isBlankLine(line) && !isComment(line)) readFile.push_back(line);
    }
    file.close();

    return readFile;
}

bool isAssigner (const string& second_token) {
    return (second_token == "=" || second_token == "+=" || second_token == "-=");
}

bool isContainerOp (const string& second_token) {
    return (second_token == "get" || second_token == "put" || second_token == "del" || second_token == "top" || second_token == "end"
        || second_token == "psh" || second_token == "pop" || second_token == "deq"
        || second_token == "num" || second_token == "igtk" || second_token == "igtv");
}

bool isFunctionCall (const string& third_token) {
    int n = third_token.size();
    return (third_token[n-1] == ':');
}

map<string,int> mapFunctions () {
    map<string,int> ret;
    for (int i = 0; i < program.size(); i++) {
        if (program[i].indentLevel == 0) ret[program[i].code] = i;
    }
    return ret;
}

int skipConditional (int line, const vector<TLine>& program) {
    int condIndentation = program[line].indentLevel;
    line++;
    while (program[line].indentLevel > condIndentation) {
        line++;
    }
    return line-1;
}

bool existsVariable (const map<string,any>& variables, const string& varName) {
    const auto& it = variables.find(varName);
    return it != variables.end();
}

bool contains (const vector<TLoop>& loopLines, int line) {
    for (auto l : loopLines) {
        if (l.line == line) return true;
    }
    return false;
}

void updateVariable (const map<string, any>& vars, mutex_map<string, any>& globVars, const string& var) {
    if (globVars.contains(var) && existsVariable(vars, var)) {
        globVars.add_or_set(var, vars.at(var));
    }
}

void updateVariables (const mutex_map<string, any>& globVars, map<string, any>& vars) {
    // Also dumps new variables that may have been created in src, for global
    for (auto& [key, value] : globVars.snapshot()) {
        vars[key] = value;
    }
}

///////////////////////////////////////////////////////////
// PROCEDURES
///////////////////////////////////////////////////////////

vector<TLine> parse (int startln, const vector<string>& readFile) {
    vector<TLine> parsedProgram;
    int ln = startln;

    for (int i = 0; i < readFile.size(); i++) {
        int tabs = 0;
        vector<string> tokens = split(tabs, readFile[i]);

        //for (string t: tokens) cout << t << " ";
        //cout << tokens.size() << endl;

        if (tokens[0] == "import") {
            for (int j = 1; j < tokens.size(); j++) {
                // Now, we don't create duplicated imports with this implementation
                importStatements.insert(tokens[j]);
            }
        }
        else if (tokens[0] == "require") {
            for (int j = 1; j < tokens.size(); j++) {
                int n = tokens[j].size();
                if (tokens[j][n-4] != '.' || tokens[j][n-3] != 'l' || tokens[j][n-2] != 't' || tokens[j][n-1] != 's') {
                    cerr << "Indicated file is not a LiteScript file. (.lts)" << endl;
                    exit(1);
                }

                vector<string> readFile = readClean(tokens[j]);
                vector<TLine> requiredProgram = parse(ln, readFile);
                for (int k = 0; k < requiredProgram.size(); k++) {
                    parsedProgram.push_back(requiredProgram[k]);
                    ln++;
                }
            }
        }
        else {
            if (tokens[0] == "conditional" || tokens[0] == "loop") {
                string conditional_expr;
                for (int j = 1; j < tokens.size(); j++) {
                    conditional_expr += tokens[j];
                }
                parsedProgram.push_back(TLine{tabs, tokens[0] + " " + conditional_expr});
            }
            else if (tokens.size() > 2 && isAssigner(tokens[1]) && !isFunctionCall(tokens[2])) {
                string arithmetic_expr;
                for (int j = 2; j < tokens.size(); j++) {
                    arithmetic_expr += tokens[j];
                }
                parsedProgram.push_back(TLine{tabs, tokens[0] + " " + tokens[1] + " " + arithmetic_expr});
            }
            else {
                string remerge = tokens[0];
                for (int j = 1; j < tokens.size(); j++) {
                    remerge += " " + tokens[j];
                }
                parsedProgram.push_back(TLine{tabs, remerge});
            }

            ln++;
        }
    }

    return parsedProgram;
}

// NOTES:
// Args are always passed by const reference.

// PRE: Program with valid LTS syntax
// POST: Expected LTS behavior (without bugs, hopefully...)
any interpret (int startLine, const vector<any>& args) {

    // === BUILD PHASE ===

    // We need to take into account these two DS when creating variables, we need to force
    // the evaluator to not create empty variables.
    stack<vector<string>> variableStack;
    map<string,any> variables;

    // Dump global variables to local variables at start of execution
    for (auto& [key, value] : globalVariables.snapshot()) {
        variables[key] = value;
        // No need to add it to the variable stack, once a global variable is declared it cannot
        // be destroyed until end of program
    }
    
    // Pass function arguments to local variables
    for (int i = 0; i < args.size(); i++) {
        string argi = "arg" + to_string(i);
        variables[argi] = args[i];
    }
    variables["args"] = int(args.size());

    int prevIndentLevel = 0;
    vector<TLoop> loopLines;

    // === RUNTIME PHASE ===
    //                         indentLevel 0 indicates eoFn             Handle eoFn with valid loops
    for (int line = startLine; !((line >= program.size() && loopLines.empty()) || (program[line].indentLevel == 0 && loopLines.empty())) ; line++) {

        /*
        cout << "line: " << program[line].code << endl;
        cout << "globalVariables:";
        for (auto& [key, value] : globalVariables) {
            cout << " {" << key << "," << any_cast<int>(value) << "}";
        }
        cout << endl;
        cout << "localVariables:";
        for (auto& [key, value] : variables) {
            cout << " {" << key << "," << any_cast<int>(value) << "}";
        }
        cout << endl;
        */

        // Handle reentering loop indendation or more
        if (!loopLines.empty() && program[line].indentLevel <= loopLines.back().indentLevel) {
            line = loopLines.back().line;
        }

        // Handle indentation decrement
        if (program[line].indentLevel < prevIndentLevel) {
            vector<string> currentScopeVars = variableStack.top();
            for (string var: currentScopeVars) {
                variables.erase(var);
            }
            variableStack.pop();
        }
        // Handle indentation increment
        else if (program[line].indentLevel > prevIndentLevel) {
            variableStack.push(vector<string>());
        }
        prevIndentLevel = program[line].indentLevel;
        
        //cout << program[line].code << endl;

        int dummyTabs; // To be ignored
        vector<string> instruction = split(dummyTabs, program[line].code);



        // ASSIGNMENTS
        if (instruction.size() >= 3 && isAssigner(instruction[1])) {
            any value;
            // Handle function call first then expressions
            if (isFunctionCall(instruction[2])) {
                string funcName = instruction[2];
                funcName.pop_back();
                vector<any> funcArgs;
                for (int i = 3; i < instruction.size(); i++) funcArgs.push_back(evaluate(variables,instruction[i]));
                value = interpret(funcs.at(funcName)+1, funcArgs);
            }
            else {
                value = evaluate(variables, instruction[2]);
            }

            // Assign (have to use casts)
            if (instruction[1] == "=") {
                // In this case, we are allowed to use [], creates if doesnt exist
                if (!existsVariable(variables, instruction[0])) variableStack.top().push_back(instruction[0]);
                variables[instruction[0]] = value;
            }
            else if (instruction[1] == "+=") {
                if (variables.at(instruction[0]).type() == typeid(int) && value.type() == typeid(int)) {
                    int a = any_cast<int>(variables.at(instruction[0]));
                    int b = any_cast<int>(value);
                    a += b;
                    variables.at(instruction[0]) = a;
                }
                else if (variables.at(instruction[0]).type() == typeid(float) && value.type() == typeid(float)) {
                    float a = any_cast<float>(variables.at(instruction[0]));
                    float b = any_cast<float>(value);
                    a += b;
                    variables.at(instruction[0]) = a;
                }
                else {
                    string a = any_cast<string>(variables.at(instruction[0]));
                    string b = any_cast<string>(value);
                    a = a + b;
                    variables.at(instruction[0]) = a;
                }
            }
            else {
                if (variables.at(instruction[0]).type() == typeid(int) && value.type() == typeid(int)) {
                    int a = any_cast<int>(variables.at(instruction[0]));
                    int b = any_cast<int>(value);
                    a -= b;
                    variables.at(instruction[0]) = a;
                }
                else {
                    float a = any_cast<float>(variables.at(instruction[0]));
                    float b = any_cast<float>(value);
                    a -= b;
                    variables.at(instruction[0]) = a;
                }
            }

            // Update global variables if exists
            updateVariable(variables, globalVariables, instruction[0]);
        }



        // CONTAINER (key can be expr) (all expr must be condensed)
        else if (instruction[0] == "container") {
            map<Key,any> cont;
            if (!existsVariable(variables, instruction[1])) variableStack.top().push_back(instruction[1]);
            variables[instruction[1]] = cont;

            // Update global variables if exists
            updateVariable(variables, globalVariables, instruction[1]);
        }
        else if (instruction.size() >= 2 && isContainerOp(instruction[1])) {
            if (instruction[1] == "get") {
                // c get key -> var
                any key = evaluate(variables, instruction[2]);
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                if (key.type() == typeid(int)) variables.at(instruction[4]) = cont[Key{any_cast<int>(key)}];
                else if (key.type() == typeid(float)) variables.at(instruction[4]) = cont[Key{any_cast<float>(key)}];
                else variables.at(instruction[4]) = cont[Key{any_cast<string>(key)}];
            }
            else if (instruction[1] == "put") {
                // c put key <- expr
                any key = evaluate(variables, instruction[2]);
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                if (key.type() == typeid(int)) cont[Key{any_cast<int>(key)}] = evaluate(variables, instruction[4]);
                else if (key.type() == typeid(float)) cont[Key{any_cast<float>(key)}] = evaluate(variables, instruction[4]);
                else cont[Key{any_cast<string>(key)}] = evaluate(variables, instruction[4]);
            }
            else if (instruction[1] == "del") {
                // c del key
                any key = evaluate(variables, instruction[2]);
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                if (key.type() == typeid(int)) cont.erase(Key{any_cast<int>(key)});
                else if (key.type() == typeid(float)) cont.erase(Key{any_cast<float>(key)});
                else cont.erase(Key{any_cast<string>(key)});
            }
            else if (instruction[1] == "top") {
                // c top -> var
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                auto it = cont.begin();
                variables.at(instruction[3]) = it->second;
            }
            else if (instruction[1] == "end") {
                // c end -> var
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                auto it = cont.end();
                it--;
                any val = it->second;
                variables.at(instruction[3]) = val;
            }
            else if (instruction[1] == "psh") {
                // c psh <- expr
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                if (cont.empty()) {
                    cont[Key{int(0)}] = evaluate(variables, instruction[3]);
                }
                else {
                    auto it = cont.end();
                    it--;
                    variant<int, float, string> key = it->first.value;
                    if (key.index() != 0) {
                        cerr << "'psh' operator cannot be applied to a non-numerical indexed container." << endl;
                        exit(1);
                    }
                    int newKey = get<int>(key)+1;
                    cont[Key{any_cast<int>(newKey)}] = evaluate(variables, instruction[3]);
                }
            }
            else if (instruction[1] == "pop") {
                // c pop
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                auto it = cont.end();
                it--;
                cont.erase(it);
            }
            else if (instruction[1] == "deq") {
                // c deq
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                auto it = cont.begin();
                cont.erase(it);
            }
            else if (instruction[1] == "num") {
                // c num -> var
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                int num = cont.size();
                variables.at(instruction[3]) = num;
            }
            else if (instruction[1] == "igtk") {
                // NOTE: igtk = indexed get key; expr has to eval to an integer
                // c igtk expr -> var
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                any key = evaluate(variables, instruction[2]);
                if (key.type() != typeid(int)) {
                    cerr << "igtk key does not correspon to an indexable value" << endl;
                    exit(1);
                }
                int idx = any_cast<int>(key);
                auto it = cont.begin();
                advance(it, idx);
                Key ky = it->first;
                any rt;
                if (auto p = get_if<int>(&ky.value)) {
                    rt = *p;
                }
                else if (auto p = get_if<float>(&ky.value)) {
                    rt = *p;
                }
                else if (auto p = get_if<string>(&ky.value)) {
                    rt = *p;
                }
                else {
                    cerr << "Variant key type error" << endl;
                    exit(1);
                }
                variables.at(instruction[4]) = rt;
            }
            else if (instruction[1] == "igtv") {
                // NOTE: igtk = indexed get val; expr has to eval to an integer
                // c igtv expr -> var
                auto& cont = any_cast<map<Key,any>&>(variables.at(instruction[0]));
                any key = evaluate(variables, instruction[2]);
                if (key.type() != typeid(int)) {
                    cerr << "igtv key does not correspon to an indexable value" << endl;
                    exit(1);
                }
                int idx = any_cast<int>(key);
                auto it = cont.begin();
                advance(it, idx);
                any rt = it->second;
                variables.at(instruction[4]) = rt;
            }

            // Update global variables if exists
            updateVariable(variables, globalVariables, instruction[0]);
        }



        // CONDITIONAL
        else if (instruction[0] == "conditional") {
            any cond = evaluate(variables, instruction[1]);
            bool isFalse = false;
            if (cond.type() == typeid(int)) isFalse = (any_cast<int>(cond) == 0);
            else if (cond.type() == typeid(float)) isFalse = (any_cast<float>(cond) == 0.0f);
            else isFalse = (any_cast<string>(cond).empty());

            if (isFalse) line = skipConditional(line, program);
        }



        // LOOP
        else if (instruction[0] == "loop") {
            any cond = evaluate(variables, instruction[1]);
            bool isFalse = false;
            if (cond.type() == typeid(int)) isFalse = (any_cast<int>(cond) == 0);
            else if (cond.type() == typeid(float)) isFalse = (any_cast<float>(cond) == 0.0f);
            else isFalse = (any_cast<string>(cond).empty());

            if (isFalse) {
                line = skipConditional(line, program);
                if (!loopLines.empty()) loopLines.pop_back();
            }
            else {
                if (loopLines.empty()) loopLines.push_back(TLoop{line, program[line].indentLevel});
                else if (!contains(loopLines,line)) loopLines.push_back(TLoop{line, program[line].indentLevel});
            }
        }



        // RETURN
        else if (instruction[0] == "return") {
            any ret;
            if (!existsVariable(variables, instruction[1])) ret = evaluate(variables, instruction[1]);
            else ret = variables.at(instruction[1]);
            return ret;
        }



        // FUNCTION CALL (void or ignored return value)
        else if (isFunctionCall(instruction[0])) {
            string funcName = instruction[0];
            funcName.pop_back();
            vector<any> funcArgs;
            for (int i = 1; i < instruction.size(); i++) funcArgs.push_back(evaluate(variables,instruction[i]));
            interpret(funcs.at(funcName)+1, funcArgs);

            // Update local variables
            updateVariables(globalVariables, variables);
        }



        // EXIT
        else if (instruction[0] == "exit") {
            if (instruction.size() == 1) exit(0);
            else exit(stoi(instruction[1]));
        }



        // GLOBAL VARIABLE DECLARATION
        else if (instruction[0] == "global") {
            globalVariables.add_or_set(instruction[1], any());
        }



        // IMPORTED FUNCTIONS
        else {
            bool executed = false;
            vector<any> importArgs;
            deque<any> exprArgs;
            for (int j = 1; j < instruction.size(); j++) {
                if (!existsVariable(variables, instruction[j])) {
                    if (isFunctionCall(instruction[j])) {
                        // Here we encounter a function call, parse all the other remaining
                        // arguments as arguments of the function call and insert a interpret() ref.
                        InterpretFunc fp = &interpret;
                        string fun = instruction[j];
                        fun.pop_back();
                        int interpretLine = funcs.at(fun);
                        vector<any> interpretArgs;
                        for (j = j+1; j < instruction.size(); j++) {
                            interpretArgs.push_back(evaluate(variables, instruction[j]));
                        }
                        any stored = storedInterpret{fp, interpretLine+1, interpretArgs};
                        importArgs.push_back(stored);
                    }
                    else {
                        // Here, it is treated as an expression, we have an auxiliary vector
                        // to store references to these temporary values
                        exprArgs.push_back(evaluate(variables, instruction[j]));
                        if (exprArgs.back().type() == typeid(int)) {
                            importArgs.push_back(ref(any_cast<int&>(exprArgs.back())));
                        }
                        else if (exprArgs.back().type() == typeid(float)) {
                            importArgs.push_back(ref(any_cast<float&>(exprArgs.back())));
                        }
                        else {
                            importArgs.push_back(ref(any_cast<string&>(exprArgs.back())));
                        }
                    }
                }
                else if (variables.at(instruction[j]).type() == typeid(int)) {
                    importArgs.push_back(ref(any_cast<int&>(variables.at(instruction[j]))));
                }
                else if (variables.at(instruction[j]).type() == typeid(float)) {
                    importArgs.push_back(ref(any_cast<float&>(variables.at(instruction[j]))));
                }
                else if (variables.at(instruction[j]).type() == typeid(map<Key,any>)){
                    importArgs.push_back(ref(any_cast<map<Key,any>&>(variables.at(instruction[j]))));
                }
                else {
                    importArgs.push_back(ref(any_cast<string&>(variables.at(instruction[j]))));
                }
            }

            for (int i = 0; i < importedFunctions.size() && !executed; i++) {
                executed = importedFunctions[i]->execute(instruction[0], importArgs);
            }

            // Left instruction is either not imported or non-existent
            if (!executed) {
                cerr << "Unrecognized instruction: " << program[line].code << endl;
                exit(1);
            }

            // Update local variables
            updateVariables(globalVariables, variables);
        }
    }

    // === END OF FUNCTION ===

    return any{}; // return nothing when eoFn
}

/////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////

int main (int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename/absolute path to file> <additional args...>" << endl;
        cerr << "If filename given then it will search in the current working directory." << endl; 
        return 1;
    }

    if (string(argv[1]) == "--version") {
        cout << "lite 0.3.0 2026-1-22" << endl;
        return 0;
    }

    string filePath = argv[1];
    int n = filePath.size();
    if (filePath[n-4] != '.' || filePath[n-3] != 'l' || filePath[n-2] != 't' || filePath[n-1] != 's') {
        cerr << "Indicated file is not a LiteScript file. (.lts)" << endl;
        return 1;
    }

    vector<string> readFile = readClean(filePath);

    //for (string r: readFile) cout << r << endl;

    program = parse(0, readFile);

    /*
    cout << "L I code" << endl;
    cout << "--------" << endl; 
    for (int i = 0; i < program.size(); i++) {
        cout << i << " " << program[i].indentLevel << " " << program[i].code << endl;
    }
    */
    
    // Create imported functions
    FunctionFactory& factory = FunctionFactory::getInstance();
    importedFunctions = factory.createFunctions(importStatements);

    funcs = mapFunctions();
    int startLine = funcs.at("start");
    vector<any> programArgs;
    for (int i = 2; i < argc; i++) programArgs.push_back(string(argv[i]));
    interpret(startLine+1, programArgs);
}