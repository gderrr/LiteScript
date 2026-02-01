#include <iostream>
#include "httplib.h"
using namespace std;

int main () {
    string host = "httpbin.org";
    string path = "/get";

    httplib::Client cli(host.c_str());

    if (auto res = cli.Get(path.c_str())) {
        cout << res->body << endl;
    } else {
        cout << "" << endl;
    }
}