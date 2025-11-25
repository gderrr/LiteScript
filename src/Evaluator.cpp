#include "Evaluator.h"
#include <cstdlib>
#include <vector>
#include <stack>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cmath>

////////////////////////////////////////////
// UTILS
////////////////////////////////////////////

enum class TokenType { Number, Float, String, Variable, Operator, Parenthesis };

struct Token {
    TokenType type;
    string value;
};

bool isOperatorChar (char c) {
    return string("+-*/%^&|!<>=()~").find(c) != string::npos;
}

bool isDigitOrDot (char c) {
    return isdigit(c) || c == 'c';
}

// ** is exponentiation
// ~ is logarithm (base 2)
int precedence (const string& op) {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "|") return 3;
    if (op == "^") return 4;
    if (op == "&") return 5;
    if (op == "==" || op == "!=") return 6;
    if (op == "<" || op == "<=" || op == ">" || op == ">=") return 7;
    if (op == "<<" || op == ">>") return 8;
    if (op == "+" || op == "-") return 9;
    if (op == "*" || op == "/" || op == "%") return 10;
    if (op == "**") return 11;
    if (op == "!" || op == "u-" || op == "~") return 12;
    return 0;
}

bool isRightAssociative (const string& op) {
    return (op == "u-" || op == "!" || op == "~" || op == "**");
}

void DEBUG_PRINT_TOKEN (const Token& tok) {
    string type;
    if (tok.type == TokenType::Number) type = "Number";
    else if (tok.type == TokenType::Float) type = "Float";
    else if (tok.type == TokenType::String) type = "String";
    else if (tok.type == TokenType::Variable) type = "Variable";
    else if (tok.type == TokenType::Operator) type = "Operator";
    else type = "Parenthesis";
    cout << "{Type: " << type << ", Value: " << tok.value << "}" << endl;
}

void DEBUG_PRINT_TOKENS (const vector<Token>& tokens) {
    int i = 0;
    for (const auto& tok: tokens) {
        cout << "Token " << i << ": ";
        DEBUG_PRINT_TOKEN(tok);
        i++;
    }
}

void DEBUG_PRINT_EVAL (const Token& tok, const stack<any>& s) {
    stack<any> st = s;
    cout << "Token: ";
    DEBUG_PRINT_TOKEN(tok);
    cout << "Stack: ";
    while (!st.empty()) {
        if (st.top().type() == typeid(int)) cout << "int{" << any_cast<int>(st.top()) << "} ";
        else if (st.top().type() == typeid(float)) cout << "float{" << any_cast<float>(st.top()) << "} ";
        else if (st.top().type() == typeid(string)) cout << "string{" << any_cast<string>(st.top()) << "} ";
        else cout << "IncompType(" << st.top().type().name() << ") ";
        st.pop();
    }
    cout << endl;
}

////////////////////////////////////////////
// EVALUATOR
////////////////////////////////////////////

// Tokenizer
vector<Token> tokenize (const string& expr) {
    vector<Token> tokens;
    size_t i = 0;
    while (i < expr.size()) {
        // Harmless, might change expression parsing later to include spaces
        // Also useful for debug
        if (isspace(expr[i])) { i++; continue; }

        // Number (int or float)
        if (isdigit(expr[i]) || expr[i] == '.') {
            size_t start = i;
            i++; // skip first char
            while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.')) i++;
            string tok = expr.substr(start, i - start);
            tokens.push_back(tok.find('.') != string::npos ? Token{TokenType::Float, tok} : Token{TokenType::Number, tok});
            continue;
        }

        // String
        if (expr[i] == '"') {
            i++;
            string val;
            while (i < expr.size() && expr[i] != '"') {
                if (expr[i] == '\\' && i+1 < expr.size()) {
                    char next = expr[++i];
                    switch (next) {
                        case 'n': val.push_back('\n'); break;
                        case 'r': val.push_back('\r'); break;
                        case 't': val.push_back('\t'); break;
                        case '\\': val.push_back('\\'); break; 
                        case '"': val.push_back('"'); break;
                        case '0': val.push_back('\0'); break;
                        case '_': val.push_back('_'); break; // Since spaces are squashed, '_' in lts rep. spaces and '\_' rep. _ in strings
                        default: val.push_back(next); break; // unknown
                    }
                } else if (expr[i] == '_') {
                    val.push_back(' ');
                } else {
                    val.push_back(expr[i]);
                }
                i++;
            }
            if (i >= expr.size() || expr[i] != '"') {
                cerr << "Unterminated string literal: " << expr << endl;
                exit(1);
            }
            i++;
            tokens.push_back(Token{TokenType::String, val});
            continue;
        }

        // Parenthesis
        if (expr[i] == '(' || expr[i] == ')') {
            tokens.push_back(Token{TokenType::Parenthesis, string(1, expr[i])});
            i++;
            continue;
        }

        // Operator, possibly two characters long
        if (isOperatorChar(expr[i])) {
            string op(1, expr[i]);
            if (i+1 < expr.size()) {
                string two = op + expr[i+1];
                // multi-char ops
                if (two == "==" || two == "!=" || two == "<=" || two == ">=" ||
                    two == "<<" || two == ">>" || two == "&&" || two == "||" || two == "**") {
                    op = two;
                    i++;
                }
            }
            tokens.push_back(Token{TokenType::Operator, op});
            i++;
            continue;
        }

        // Variable
        size_t start = i;
        while (i < expr.size() && (isalnum(expr[i]) || expr[i]=='_')) i++;
        tokens.push_back(Token{TokenType::Variable, expr.substr(start, i - start)});
    }
    //DEBUG_PRINT_TOKENS(tokens);
    return tokens;
}

// Shunting Yard
vector<Token> toPostfix (const vector<Token>& tokens) {
    vector<Token> output;
    stack<Token> ops;
    bool expectUnary = true;

    for (const auto& tok : tokens) {
        if (tok.type == TokenType::Number || tok.type == TokenType::Float ||
            tok.type == TokenType::String || tok.type == TokenType::Variable) {
            output.push_back(tok);
            expectUnary = false;
        }
        else if (tok.type == TokenType::Operator) {
            string op = tok.value;

            // Unary handling
            if (op == "-" && expectUnary) op = "u-";
            else if (op == "+" && expectUnary) continue;

            // Precedence handling
            while (!ops.empty() && ops.top().type == TokenType::Operator &&
                    ((precedence(ops.top().value) > precedence(op)) ||
                    (precedence(ops.top().value) == precedence(op) && !isRightAssociative(op)))) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(Token{TokenType::Operator, op});
            expectUnary = true;
        }
        else if (tok.type == TokenType::Parenthesis) {
            if (tok.value == "(") {
                ops.push(tok);
                expectUnary = true;
            } else {
                while (!ops.empty() && ops.top().value != "(") {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.pop(); // remove (
                expectUnary = false;
            }
        }
    }

    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }
    //DEBUG_PRINT_TOKENS(output);
    return output;
}

// Postfix evaluator
any evalPostfix (const vector<Token>& postfix, const map<string, any>& vars) {
    stack<any> st;

    for (auto& tok : postfix) {

        if (tok.type == TokenType::Number) st.push(stoi(tok.value));
        else if (tok.type == TokenType::Float) st.push(stof(tok.value));
        else if (tok.type == TokenType::String) st.push(tok.value);
        else if (tok.type == TokenType::Variable) st.push(vars.at(tok.value));
        else if (tok.type == TokenType::Operator) {
            string op = tok.value;

            // Unary operators
            if (op == "u-") {
                any a = st.top(); st.pop();
                if (a.type() == typeid(int)) st.push(-any_cast<int>(a));
                else if (a.type() == typeid(float)) st.push(-any_cast<float>(a));
                else {
                    cerr << "Invalid -u operand type" << endl;
                    exit(1); 
                }
                continue;
            }
            if (op == "!") {
                any a = st.top(); st.pop();
                int val;
                if (a.type() == typeid(int)) val = any_cast<int>(a);
                else if (a.type() == typeid(float)) val = any_cast<float>(a) != 0.0f ? 1 : 0;
                else {
                    cerr << "Invalid ! operand type" << endl;
                    exit(1);
                }
                st.push(val == 0 ? 1 : 0);
                continue;
            }
            if (op == "~") {
                any a = st.top(); st.pop();

                float val;
                if (a.type() == typeid(int)) val = static_cast<float>(any_cast<int>(a));
                else val = any_cast<float>(a);
                st.push(log2(val));
                continue;
            }

            // Binary operators
            any b = st.top(); st.pop();
            any a = st.top(); st.pop();

            // Helper to determine type
            bool aIsFloat = (a.type() == typeid(float));
            bool bIsFloat = (b.type() == typeid(float));
            bool aIsString = (a.type() == typeid(string));
            bool bIsString = (b.type() == typeid(string));

            // String operations
            if (aIsString || bIsString) {
                if (!(aIsString && bIsString)) {
                    cerr << "String operations are only compatible with other strings." << endl;
                    exit(1);
                }

                string sa = any_cast<string>(a);
                string sb = any_cast<string>(b);
                if (op == "+") st.push(sa + sb);
                else if (op == "==") st.push(sa == sb ? 1 : 0);
                else if (op == "!=") st.push(sa != sb ? 1 : 0);
                else if (op == "<") st.push(sa < sb ? 1 : 0);
                else if (op == "<=") st.push(sa <= sb ? 1 : 0);
                else if (op == ">") st.push(sa > sb ? 1 : 0);
                else if (op == ">=") st.push(sa >= sb ? 1 : 0);
                else {
                    cerr << "Invalid string operator: " << op << endl;
                    exit(1);
                }
                continue;
            }

            // Float operations
            if (aIsFloat || bIsFloat) {
                float fa = aIsFloat ? any_cast<float>(a) : float(any_cast<int>(a));
                float fb = bIsFloat ? any_cast<float>(b) : float(any_cast<int>(b));
                if (op == "+") st.push(fa + fb);
                else if (op == "-") st.push(fa - fb);
                else if (op == "*") st.push(fa * fb);
                else if (op == "/") st.push(fa / fb);
                else if (op == "**") st.push(pow(fa, fb));
                else if (op == "==") st.push(fa == fb ? 1 : 0);
                else if (op == "!=") st.push(fa != fb ? 1 : 0);
                else if (op == "<") st.push(fa < fb ? 1 : 0);
                else if (op == "<=") st.push(fa <= fb ? 1 : 0);
                else if (op == ">") st.push(fa > fb ? 1 : 0);
                else if (op == ">=") st.push(fa >= fb ? 1 : 0);
                else {
                    cerr << "Invalid float operator: " << op << endl;
                    exit(1);
                }
                continue;
            }

            // Integer operations
            int ai = any_cast<int>(a);
            int bi = any_cast<int>(b);

            if (op == "+") st.push(ai + bi);
            else if (op == "-") st.push(ai - bi);
            else if (op == "*") st.push(ai * bi);
            else if (op == "/") st.push(ai / bi);
            else if (op == "%") st.push(ai % bi);
            else if (op == "|") st.push(ai | bi);
            else if (op == "&") st.push(ai & bi);
            else if (op == "^") st.push(ai ^ bi);
            else if (op == "<<") st.push(ai << bi);
            else if (op == ">>") st.push(ai >> bi);
            else if (op == "||") st.push((ai || bi) ? 1 : 0);
            else if (op == "&&") st.push((ai && bi) ? 1 : 0);
            else if (op == "==") st.push(ai == bi ? 1 : 0);
            else if (op == "!=") st.push(ai != bi ? 1 : 0);
            else if (op == "<") st.push(ai < bi ? 1 : 0);
            else if (op == "<=") st.push(ai <= bi ? 1 : 0);
            else if (op == ">") st.push(ai > bi ? 1 : 0);
            else if (op == ">=") st.push(ai >= bi ? 1 : 0);
            else {
                cerr << "Invalid integer/generic operator: " << op << endl;
                exit(1);
            }
        }

        //DEBUG_PRINT_EVAL(tok, st);

    }
    any ret = st.top();
    return ret;
}

any evaluate (const map<string,any>& variables, const string& expression) {
    auto tokens = tokenize(expression);
    //cout << "===" << endl;
    auto postfix = toPostfix(tokens);
    return evalPostfix(postfix, variables);
}