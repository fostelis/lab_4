#ifndef ORDINAL_OPERATIONS_H
#define ORDINAL_OPERATIONS_H

#include "exceptions.h"
#include "ordinal_number.h"

namespace ordinal_operations {

inline bool contains(const ordinal_number& length, const ordinal_number& index) {
    if (length.is_finite()) {
        return index.is_finite() && index.finite_part() < length.finite_part();
    }
    if (index.omega_coefficient() < length.omega_coefficient()) {
        return true;
    }
    if (index.omega_coefficient() == length.omega_coefficient()) {
        return index.finite_part() < length.finite_part();
    }
    return false;
}

inline bool can_insert_at(const ordinal_number& length, const ordinal_number& index) {
    if (contains(length, index)) {
        return true;
    }
    if (length.is_finite()) {
        return index.is_finite() && index.finite_part() == length.finite_part();
    }
    return index.omega_coefficient() == length.omega_coefficient() && index.finite_part() == length.finite_part();
}

inline bool has_last(const ordinal_number& length) {
    return length.finite_part() > 0;
}

inline ordinal_number last_index(const ordinal_number& length) {
    if (!has_last(length)) {
        throw index_out_of_range_exception("ordinal_operations: number has no last predecessor");
    }
    return ordinal_number(length.omega_coefficient(), length.finite_part() - 1);
}

inline ordinal_number add_finite(const ordinal_number& number, std::size_t value) {
    return ordinal_number(number.omega_coefficient(), number.finite_part() + value);
}

inline ordinal_number concat(const ordinal_number& left, const ordinal_number& right) {
    if (left.is_finite() && right.is_finite()) {
        return ordinal_number::finite(left.finite_part() + right.finite_part());
    }
    if (right.is_finite()) {
        return ordinal_number::omega(left.omega_coefficient(), left.finite_part() + right.finite_part());
    }
    if (left.is_finite()) {
        return right;
    }
    return ordinal_number::omega(left.omega_coefficient() + right.omega_coefficient(), right.finite_part());
}

}

#endif
