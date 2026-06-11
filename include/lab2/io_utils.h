#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <iostream>
#include "sequence.h"


template<class T>
std::ostream& operator<<(std::ostream& os, const sequence<T>& seq) {
    os << "{";
    for (int i = 0; i < seq.get_length(); ++i) {
        os << seq.get(i);
        if (i < seq.get_length() - 1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

#endif
