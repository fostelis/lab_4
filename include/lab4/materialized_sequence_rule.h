#ifndef MATERIALIZED_SEQUENCE_RULE_H
#define MATERIALIZED_SEQUENCE_RULE_H

#include <cstddef>
#include <memory>
#include "exceptions.h"
#include "sequence.h"
#include "array_sequence.h"
#include "element_generation_rule.h"

template<class T>
class materialized_sequence_rule : public element_generation_rule<T> {
private:
    std::shared_ptr<immutable_array_sequence<T>> stored_elements_;

public:
    materialized_sequence_rule()
        : stored_elements_(std::make_shared<immutable_array_sequence<T>>()) {}

    materialized_sequence_rule(const T* items, int count)
        : stored_elements_(std::make_shared<immutable_array_sequence<T>>(items, count)) {}

    explicit materialized_sequence_rule(const sequence<T>& source_sequence)
        : stored_elements_(std::make_shared<immutable_array_sequence<T>>()) {
        sequence<T>* current = stored_elements_->clone();
        for (int i = 0; i < source_sequence.get_length(); ++i) {
            sequence<T>* next = current->append(source_sequence.get(i));
            if (next != current) {
                delete current;
            }
            current = next;
        }
        stored_elements_.reset(dynamic_cast<immutable_array_sequence<T>*>(current));
    }

    T get(const ordinal_number& index) const override {
        if (!index.is_finite() || index.finite_part() >= static_cast<std::size_t>(stored_elements_->get_length())) {
            throw index_out_of_range_exception("lazy_sequence: index out of range");
        }
        return stored_elements_->get(static_cast<int>(index.finite_part()));
    }

    ordinal_number length() const override {
        return ordinal_number::finite(static_cast<std::size_t>(stored_elements_->get_length()));
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<materialized_sequence_rule<T>>(*stored_elements_);
    }
};

#endif
