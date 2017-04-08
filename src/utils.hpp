#ifndef UTILS_HPP
#define UTILS_HPP


#include <string>
#include <iostream>


using std::string;
using std::to_string;
using std::ostream;


#define REPR(T) \
    string repr(const T &value); \
    inline ostream &operator <<(ostream &os, const T &value) { \
        os << repr(value); \
        return os; \
    } \
    inline string repr(const T &value)


inline string repr(int value) {
    return to_string(value);
}
inline string repr(long value) {
    return to_string(value);
}
inline string repr(long long value) {
    return to_string(value);
}
inline string repr(unsigned value) {
    return to_string(value);
}
inline string repr(unsigned long value) {
    return to_string(value);
}
inline string repr(unsigned long long value) {
    return to_string(value);
}
inline string repr(float value) {
    return to_string(value);
}
inline string repr(double value) {
    return to_string(value);
}
inline string repr(long double value) {
    return to_string(value);
}


#endif // UTILS_HPP
