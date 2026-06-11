#ifndef ORDINAL_NUMBER_H
#define ORDINAL_NUMBER_H

#include <cstddef>
#include "exceptions.h"

class ordinal_number {
private:
    std::size_t omega_coefficient_;
    std::size_t finite_part_;

public:
    ordinal_number() : omega_coefficient_(0), finite_part_(0) {}
    ordinal_number(std::size_t omega_coefficient, std::size_t finite_part)
        : omega_coefficient_(omega_coefficient), finite_part_(finite_part) {}

    static ordinal_number finite(std::size_t value) {
        return ordinal_number(0, value);
    }

    static ordinal_number omega(std::size_t coefficient = 1, std::size_t finite_part = 0) {
        return ordinal_number(coefficient, finite_part);
    }

    bool is_finite() const {
        return omega_coefficient_ == 0;
    }

    bool has_omega_part() const {
        return omega_coefficient_ > 0;
    }

    std::size_t finite_value() const {
        if (!is_finite()) {
            throw index_out_of_range_exception("ordinal_number: value is not finite");
        }
        return finite_part_;
    }

    std::size_t omega_coefficient() const {
        return omega_coefficient_;
    }

    std::size_t finite_part() const {
        return finite_part_;
    }

    bool equals(const ordinal_number& other) const {
        return omega_coefficient_ == other.omega_coefficient_ && finite_part_ == other.finite_part_;
    }
};

#endif
