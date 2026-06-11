#ifndef RECURRENT_SEQUENCE_RULE_H
#define RECURRENT_SEQUENCE_RULE_H

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include "exceptions.h"
#include "sequence.h"
#include "array_sequence.h"
#include "generation_window.h"
#include "element_generation_rule.h"


template<class T>
class recurrent_sequence_rule : public element_generation_rule<T> {
private:
    std::function<T(const sequence<T>&, std::size_t)> next_element_rule_;
    std::optional<std::size_t> finite_length_;
    mutable generation_window<T> generation_window_;
    mutable std::size_t next_index_to_generate_ = 0;

    void generate_until(std::size_t target_index) const {
        if (finite_length_ && target_index >= *finite_length_) {
            throw index_out_of_range_exception("lazy_sequence: index out of range");
        }
        if (target_index < next_index_to_generate_ && !generation_window_.contains(target_index)) {
            throw index_out_of_range_exception("lazy_sequence: requested element is outside generation window");
        }
        while (next_index_to_generate_ <= target_index) {
            mutable_array_sequence<T> window_as_sequence = generation_window_.to_sequence();
            T next_value = next_element_rule_(window_as_sequence, next_index_to_generate_);
            generation_window_.push_back(next_value);
            ++next_index_to_generate_;
        }
    }

public:
    recurrent_sequence_rule(std::function<T(const sequence<T>&, std::size_t)> next_element_rule,
                            const sequence<T>& seed,
                            std::optional<std::size_t> finite_length,
                            std::size_t generation_window_capacity)
        : next_element_rule_(std::move(next_element_rule)),
          finite_length_(finite_length),
          generation_window_(generation_window_capacity == 0 ? 1 : generation_window_capacity) {
        if (finite_length_ && *finite_length_ < static_cast<std::size_t>(seed.get_length())) {
            throw index_out_of_range_exception("lazy_sequence: finite length smaller than seed");
        }
        for (int i = 0; i < seed.get_length(); ++i) {
            generation_window_.push_back(seed.get(i));
            ++next_index_to_generate_;
        }
    }

    T get(const ordinal_number& index) const override {
        if (!index.is_finite()) {
            throw index_out_of_range_exception("lazy_sequence: recurrent sequence supports only its own omega block");
        }
        generate_until(index.finite_part());
        return generation_window_.get(index.finite_part());
    }

    ordinal_number length() const override {
        return finite_length_ ? ordinal_number::finite(*finite_length_) : ordinal_number::omega();
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        mutable_array_sequence<T> seed = generation_window_.to_sequence();
        return std::make_shared<recurrent_sequence_rule<T>>(
            next_element_rule_, seed, finite_length_, generation_window_.capacity()
        );
    }

    std::size_t materialized_count() const {
        return generation_window_.size();
    }
};

#endif
