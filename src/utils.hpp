#ifndef UTILS_HPP
#define UTILS_HPP


#include <string>
#include <iostream>


using namespace std;


#define REPR(T) \
    string repr(const T &value); \
    inline ostream &operator <<(ostream &os, const T &value) { \
        os << repr(value); \
        return os; \
    } \
    inline string repr(const T &value)


#endif // UTILS_HPP
