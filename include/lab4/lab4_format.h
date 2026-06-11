#ifndef LAB4_FORMAT_H
#define LAB4_FORMAT_H

#include <ostream>
#include "ordinal_number.h"
#include "online_statistics.h"

inline std::ostream& operator<<(std::ostream& os, const ordinal_number& number) {
    if (number.is_finite()) {
        return os << number.finite_part();
    }
    os << "omega";
    if (number.omega_coefficient() != 1) {
        os << "*" << number.omega_coefficient();
    }
    if (number.finite_part() != 0) {
        os << "+" << number.finite_part();
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const online_statistics_snapshot& snapshot) {
    os << "count=" << snapshot.count << ", min=" << snapshot.min << ", max=" << snapshot.max
       << ", mean=" << snapshot.mean << ", median=" << snapshot.median;
    return os;
}

#endif
