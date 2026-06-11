#ifndef INTERLEAVED_SEQUENCE_RULE_H
#define INTERLEAVED_SEQUENCE_RULE_H

#include <cstddef>
#include <memory>
#include "exceptions.h"
#include "array_sequence.h"
#include "element_generation_rule.h"

template<class T>
class interleaved_sequences_rule : public element_generation_rule<T> {
private:
    mutable_array_sequence<std::shared_ptr<element_generation_rule<T>>> rules_;

    std::size_t sequence_count() const {
        return static_cast<std::size_t>(rules_.get_length());
    }

    std::size_t shortest_finite_prefix_length() const {
        if (rules_.get_length() == 0) {
            return 0;
        }
        bool has_finite_rule = false;
        std::size_t shortest = 0;
        for (int i = 0; i < rules_.get_length(); ++i) {
            ordinal_number current_length = rules_.get(i)->length();
            if (!current_length.has_omega_part()) {
                if (!has_finite_rule || current_length.finite_value() < shortest) {
                    shortest = current_length.finite_value();
                }
                has_finite_rule = true;
            }
        }
        if (!has_finite_rule) {
            return 0;
        }
        return shortest;
    }

public:
    explicit interleaved_sequences_rule(const sequence<std::shared_ptr<element_generation_rule<T>>>& rules) {
        if (rules.get_length() <= 0) {
            throw empty_collection_exception("interleave: no sequences provided");
        }
        for (int i = 0; i < rules.get_length(); ++i) {
            rules_.append(rules.get(i));
        }
    }

    T get(const ordinal_number& index) const override {
        if (!index.is_finite()) {
            throw index_out_of_range_exception("interleave: ordinal block index is not supported");
        }
        std::size_t count = sequence_count();
        std::size_t source_number = index.finite_part() % count;
        std::size_t source_offset = index.finite_part() / count;
        return rules_.get(static_cast<int>(source_number))->get(ordinal_number::finite(source_offset));
    }

    ordinal_number length() const override {
        for (int i = 0; i < rules_.get_length(); ++i) {
            if (rules_.get(i)->length().has_omega_part()) {
                return ordinal_number::omega();
            }
        }
        std::size_t shortest = shortest_finite_prefix_length();
        return ordinal_number::finite(shortest * sequence_count());
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<interleaved_sequences_rule<T>>(rules_);
    }
};

#endif
