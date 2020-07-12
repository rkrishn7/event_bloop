#ifndef UTILS_H
#define UTILS_H

#include <string>

#define __DEV__ 1

using namespace std;

class Utils {
    public:
        static void log(const char* msg);
        static void err(const char* msg);
        static void err(int no, const char* msg);
    private:
        Utils();
};

#endif