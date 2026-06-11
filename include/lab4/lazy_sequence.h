#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include "exceptions.h"
#include "sequence.h"
#include "array_sequence.h"
#include "ordinal_number.h"
#include "ordinal_operations.h"
#include "element_generation_rule.h"
#include "materialized_sequence_rule.h"
#include "recurrent_sequence_rule.h"
#include "sequence_transformation_rules.h"
#include "interleaved_sequence_rule.h"

template<class T>
class generator;

template<class T>
class lazy_sequence {
private:
    std::shared_ptr<element_generation_rule<T>> element_rule_;
    std::size_t generation_window_capacity_ = 50;

    lazy_sequence(std::shared_ptr<element_generation_rule<T>> rule, std::size_t generation_window_capacity)
        : element_rule_(std::move(rule)), generation_window_capacity_(generation_window_capacity == 0 ? 1 : generation_window_capacity) {}

public:
    lazy_sequence()
        : element_rule_(std::make_shared<materialized_sequence_rule<T>>()), generation_window_capacity_(50) {}

    lazy_sequence(const T* items, int count)
        : generation_window_capacity_(50) {
        if (count < 0) {
            throw index_out_of_range_exception("lazy_sequence: negative size");
        }
        if (count > 0 && items == nullptr) {
            throw index_out_of_range_exception("lazy_sequence: null array");
        }
        element_rule_ = std::make_shared<materialized_sequence_rule<T>>(items, count);
    }

    lazy_sequence(const sequence<T>& seq)
        : element_rule_(std::make_shared<materialized_sequence_rule<T>>(seq)), generation_window_capacity_(50) {}

    lazy_sequence(std::function<T(const sequence<T>&, std::size_t)> next_element_rule,
                  const sequence<T>& seed,
                  std::optional<std::size_t> finite_length = std::nullopt,
                  std::size_t generation_window_capacity = 50)
        : element_rule_(std::make_shared<recurrent_sequence_rule<T>>(
              std::move(next_element_rule), seed, finite_length, generation_window_capacity)),
          generation_window_capacity_(generation_window_capacity == 0 ? 1 : generation_window_capacity) {}

    T get_first() const {
        return get(ordinal_number::finite(0));
    }

    T get_last() const {
        ordinal_number len = get_length();
        if (!ordinal_operations::has_last(len)) {
            throw index_out_of_range_exception("lazy_sequence: sequence has no last element");
        }
        return get(ordinal_operations::last_index(len));
    }

    T get(int index) const {
        if (index < 0) {
            throw index_out_of_range_exception("lazy_sequence: negative index");
        }
        return get(ordinal_number::finite(static_cast<std::size_t>(index)));
    }

    T get(const ordinal_number& index) const {
        if (!ordinal_operations::contains(get_length(), index)) {
            throw index_out_of_range_exception("lazy_sequence: index out of range");
        }
        return element_rule_->get(index);
    }

    ordinal_number get_length() const {
        return element_rule_->length();
    }

    std::size_t get_materialized_count() const {
        auto recurrent = dynamic_cast<recurrent_sequence_rule<T>*>(element_rule_.get());
        if (recurrent) {
            return recurrent->materialized_count();
        }
        ordinal_number len = get_length();
        return len.has_omega_part() ? 0 : len.finite_value();
    }

    lazy_sequence<T> get_subsequence(int start, int end) const {
        if (start < 0 || end < start) {
            throw index_out_of_range_exception("lazy_sequence: invalid range");
        }
        mutable_array_sequence<T> data;
        for (int i = start; i <= end; ++i) {
            data.append(get(i));
        }
        return lazy_sequence<T>(data);
    }

    lazy_sequence<T> append(const T& item) const {
        return lazy_sequence<T>(std::make_shared<appended_element_rule<T>>(element_rule_, item), generation_window_capacity_);
    }

    lazy_sequence<T> prepend(const T& item) const {
        return lazy_sequence<T>(std::make_shared<prepended_element_rule<T>>(element_rule_, item), generation_window_capacity_);
    }

    lazy_sequence<T> insert_at(const T& item, int index) const {
        if (index < 0) {
            throw index_out_of_range_exception("lazy_sequence: negative insert index");
        }
        return insert_at(item, ordinal_number::finite(static_cast<std::size_t>(index)));
    }

    lazy_sequence<T> insert_at(const T& item, const ordinal_number& index) const {
        if (!ordinal_operations::can_insert_at(get_length(), index)) {
            throw index_out_of_range_exception("lazy_sequence: insert index out of range");
        }
        return lazy_sequence<T>(std::make_shared<inserted_element_rule<T>>(element_rule_, item, index), generation_window_capacity_);
    }

    lazy_sequence<T> concat(const lazy_sequence<T>& other) const {
        return lazy_sequence<T>(std::make_shared<concatenated_sequence_rule<T>>(element_rule_, other.element_rule_), generation_window_capacity_);
    }

    static lazy_sequence<T> interleave(const sequence<lazy_sequence<T>>& sequences) {
        mutable_array_sequence<std::shared_ptr<element_generation_rule<T>>> rules;
        for (int i = 0; i < sequences.get_length(); ++i) {
            rules.append(sequences.get(i).element_rule_);
        }
        return lazy_sequence<T>(std::make_shared<interleaved_sequences_rule<T>>(rules), 50);
    }

    lazy_sequence<T> interleave_with(const lazy_sequence<T>& other) const {
        mutable_array_sequence<lazy_sequence<T>> sequences;
        sequences.append(*this);
        sequences.append(other);
        return lazy_sequence<T>::interleave(sequences);
    }

    template<class U>
    lazy_sequence<U> map(std::function<U(const T&)> transform_rule) const {
        return lazy_sequence<U>(std::make_shared<mapped_sequence_rule<T, U>>(element_rule_, std::move(transform_rule)), generation_window_capacity_);
    }

    lazy_sequence<T> where(std::function<bool(const T&)> predicate, std::size_t max_scan = 100000) const {
        mutable_array_sequence<T> filtered;
        ordinal_number len = get_length();
        std::size_t limit = len.has_omega_part() ? max_scan : len.finite_value();
        for (std::size_t i = 0; i < limit; ++i) {
            T value = get(static_cast<int>(i));
            if (predicate(value)) {
                filtered.append(value);
            }
        }
        return lazy_sequence<T>(filtered);
    }

    template<class Acc>
    Acc reduce(Acc initial, std::function<Acc(const Acc&, const T&)> reduce_rule, std::size_t max_items = 100000) const {
        Acc acc = initial;
        ordinal_number len = get_length();
        std::size_t limit = len.has_omega_part() ? max_items : len.finite_value();
        for (std::size_t i = 0; i < limit; ++i) {
            acc = reduce_rule(acc, get(static_cast<int>(i)));
        }
        return acc;
    }

    lazy_sequence<std::pair<T, T>> zip(const lazy_sequence<T>& other, std::size_t max_for_infinite = 100000) const {
        ordinal_number left_len = get_length();
        ordinal_number right_len = other.get_length();
        std::size_t left = left_len.has_omega_part() ? max_for_infinite : left_len.finite_value();
        std::size_t right = right_len.has_omega_part() ? max_for_infinite : right_len.finite_value();
        std::size_t limit = std::min(left, right);
        mutable_array_sequence<std::pair<T, T>> data;
        for (std::size_t i = 0; i < limit; ++i) {
            data.append(std::make_pair(get(static_cast<int>(i)), other.get(static_cast<int>(i))));
        }
        return lazy_sequence<std::pair<T, T>>(data);
    }

    mutable_array_sequence<T> take(std::size_t count) const {
        mutable_array_sequence<T> result;
        for (std::size_t i = 0; i < count; ++i) {
            result.append(get(static_cast<int>(i)));
        }
        return result;
    }

    generator<T> get_generator() const;

    template<class U>
    friend class lazy_sequence;
};

#include "lazy_sequence_generator.h"

template<class T>
generator<T> lazy_sequence<T>::get_generator() const {
    return generator<T>(*this);
}

#endif
