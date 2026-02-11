#include <iostream>
#include <string>
using namespace std;

void injectUnderscores (string& line) {
    bool inString = false;
    for (int i = 0; i < line.size(); i++) {
        if (line[i] == '\"') inString = !inString;
        else if (line[i] == ' ' && inString) line[i] = '_';
        else if (line[i] == '_' && inString) line.insert(i++, "\\");
    }
}

int main () {
    string str = "\" _\"";
    cout << "Before: " << str << endl;
    injectUnderscores(str);
    cout << "After: " << str << endl;
}