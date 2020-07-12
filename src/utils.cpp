#include <iostream>
#include <errno.h>
#include "utils.h"

void Utils::log(const char* msg) {
    if(__DEV__)
        cout << msg << endl;
}

void Utils::err(const char* msg) {
    if(__DEV__) {
        perror(msg);
    }
}

void Utils::err(int num, const char* msg) {
    if(__DEV__) {
        errno = num;
        perror(msg);
    }
}