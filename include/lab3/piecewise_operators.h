#ifndef PIECEWISE_OPERATORS_H
#define PIECEWISE_OPERATORS_H

#include "piecewise_function.h"
#include "segment_io.h"
#include <iostream>
#include <cmath>



inline piecewise_function* operator+(const piecewise_function& lhs, const piecewise_function& rhs) {
    piecewise_function* res = lhs.clone();
    piecewise_function* concat_res = res->concat(rhs);
    if (concat_res != res) {
        delete res;
    }
    return concat_res;
}

inline bool operator==(const piecewise_function& lhs, const piecewise_function& rhs) {
    if (lhs.get_segment_count() != rhs.get_segment_count()) {
        return false;
    }
    for (int i = 0; i < lhs.get_segment_count(); ++i) {
        segment l = lhs.get_segment(i);
        segment r = rhs.get_segment(i);
        if (std::abs(l.start_x - r.start_x) > 1e-9 ||
            std::abs(l.end_x - r.end_x) > 1e-9 ||
            std::abs(l.k - r.k) > 1e-9 ||
            std::abs(l.m - r.m) > 1e-9) {
            return false;
            }
    }
    return true;
}

inline bool operator!=(const piecewise_function& lhs, const piecewise_function& rhs) {
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const piecewise_function& pf) {
    os << "PiecewiseFunction[" << pf.get_segment_count() << " segments] {\n";
    for (int i = 0; i < pf.get_segment_count(); ++i) {
        os << "  " << pf.get_segment(i) << "\n";
    }
    os << "}";
    return os;
}

#endif
