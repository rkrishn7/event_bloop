#include <iostream>
#include <string>
#include "el.h"

using namespace std;

void* test(void* a) {
    long c = (long)a;
    cout << c << endl;
}

void* filee(void* a) {
    cout << (char*)a << endl;
}

int main() {
    EventLoop el;
    el.set_timer(test, (void*)(1), 1);
    el.set_timer(test, (void*)(2), 2);
    el.set_timer(test, (void*)(3), 3);
    el.set_timer(test, (void*)(4), 4);
    el.set_timer(test, (void*)(5), 5);
    string fn = "sample";
    string fn2 = "other";
    el.stream_file_async(fn, filee);
    el.stream_file_async(fn2, filee);
    el.start();

    exit(EXIT_SUCCESS);
}