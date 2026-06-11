#ifndef LAZY_SEQUENCE_GENERATOR_H
#define LAZY_SEQUENCE_GENERATOR_H

#include <cstddef>
#include <optional>
#include "exceptions.h"
#include "ordinal_number.h"
#include "ordinal_operations.h"
#include "lazy_sequence.h"


template<class T>
class generator {
private:
    lazy_sequence<T> sequence_;
    ordinal_number current_index_;
public:
    explicit generator(const lazy_sequence<T>& sequence)
        : sequence_(sequence), current_index_(ordinal_number::finite(0)) {}

    bool has_next() const {
        return ordinal_operations::contains(sequence_.get_length(), current_index_);
    }

    T get_next() {
        if (!has_next()) {
            throw index_out_of_range_exception("generator: end of sequence");
        }
        T value = sequence_.get(current_index_);
        if (current_index_.is_finite()) {
            current_index_ = ordinal_number::finite(current_index_.finite_part() + 1);
        }
        else {
            current_index_ = ordinal_number(current_index_.omega_coefficient(), current_index_.finite_part() + 1);
        }
        return value;
    }

    std::optional<T> try_get_next() {
        if (!has_next()) {
            return std::nullopt;
        }
        return get_next();
    }

    ordinal_number get_position_index() const {
        return current_index_;
    }

    std::size_t get_position() const {
        if (!current_index_.is_finite()) {
            throw index_out_of_range_exception("generator: position is ordinal, not finite");
        }
        return current_index_.finite_part();
    }
};

#endif
