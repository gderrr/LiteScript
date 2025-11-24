#include "Evaluator.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;

int main () {
    map<string, any> vars = { {"x", 3}, {"y", 4}, {"z", 0.05f}};
    // We must take into account that "" gets parsed as PKc instead of string when creating vars
    //vars["str1"] = evaluate(vars, "\"Hello\"");
    //vars["space"] = evaluate(vars, "\"\\_\"");
    //vars["str2"] = evaluate(vars, "\"World\"");
    //vars["endl"] = evaluate(vars, "\"\\n\"");
    string expr = "x*(y+z)";
    cout << expr << endl;
    any result = evaluate(vars, expr);
    if (result.type() == typeid(int)) cout << any_cast<int>(result) << endl;
    else if (result.type() == typeid(float)) cout << any_cast<float>(result) << endl;
    else if (result.type() == typeid(string)) cout << any_cast<string>(result) << endl;
    else cout << result.type().name() << endl;
}