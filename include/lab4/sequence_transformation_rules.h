#ifndef SEQUENCE_TRANSFORMATION_RULES_H
#define SEQUENCE_TRANSFORMATION_RULES_H

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include "exceptions.h"
#include "element_generation_rule.h"

template<class T>
class prepended_element_rule : public element_generation_rule<T> {
private:
    std::shared_ptr<element_generation_rule<T>> base_rule_;
    T prepended_element_;
public:
    prepended_element_rule(std::shared_ptr<element_generation_rule<T>> base_rule, const T& item)
        : base_rule_(std::move(base_rule)), prepended_element_(item) {}

    T get(const ordinal_number& index) const override {
        if (index.is_finite()) {
            if (index.finite_part() == 0) {
                return prepended_element_;
            }
            return base_rule_->get(ordinal_number::finite(index.finite_part() - 1));
        }
        return base_rule_->get(index);
    }

    ordinal_number length() const override {
        ordinal_number old_length = base_rule_->length();
        if (old_length.has_omega_part()) {
            return old_length;
        }
        return ordinal_operations::add_finite(old_length, 1);
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<prepended_element_rule<T>>(base_rule_, prepended_element_);
    }
};


template<class T>
class appended_element_rule : public element_generation_rule<T> {
private:
    std::shared_ptr<element_generation_rule<T>> base_rule_;
    T appended_element_;
public:
    appended_element_rule(std::shared_ptr<element_generation_rule<T>> base_rule, const T& item)
        : base_rule_(std::move(base_rule)), appended_element_(item) {}

    T get(const ordinal_number& index) const override {
        ordinal_number old_length = base_rule_->length();
        if (ordinal_operations::contains(old_length, index)) {
            return base_rule_->get(index);
        }
        ordinal_number appended_index(old_length.omega_coefficient(), old_length.finite_part());
        if (index.omega_coefficient() == appended_index.omega_coefficient() && index.finite_part() == appended_index.finite_part()) {
            return appended_element_;
        }
        throw index_out_of_range_exception("lazy_sequence: index out of range");
    }

    ordinal_number length() const override {
        return ordinal_operations::add_finite(base_rule_->length(), 1);
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<appended_element_rule<T>>(base_rule_, appended_element_);
    }
};


template<class T>
class inserted_element_rule : public element_generation_rule<T> {
private:
    std::shared_ptr<element_generation_rule<T>> base_rule_;
    T inserted_element_;
    ordinal_number insertion_index_;

    static bool same_index(const ordinal_number& a, const ordinal_number& b) {
        return a.omega_coefficient() == b.omega_coefficient() && a.finite_part() == b.finite_part();
    }

public:
    inserted_element_rule(std::shared_ptr<element_generation_rule<T>> base_rule,
                          const T& item,
                          const ordinal_number& index)
        : base_rule_(std::move(base_rule)), inserted_element_(item), insertion_index_(index) {}

    T get(const ordinal_number& index) const override {
        ordinal_number old_length = base_rule_->length();
        if (!ordinal_operations::can_insert_at(old_length, insertion_index_)) {
            throw index_out_of_range_exception("lazy_sequence: insert index out of range");
        }
        if (index.omega_coefficient() == insertion_index_.omega_coefficient()) {
            if (index.finite_part() < insertion_index_.finite_part()) {
                return base_rule_->get(index);
            }
            if (same_index(index, insertion_index_)) {
                return inserted_element_;
            }
            return base_rule_->get(ordinal_number(index.omega_coefficient(), index.finite_part() - 1));
        }
        return base_rule_->get(index);
    }

    ordinal_number length() const override {
        ordinal_number old_length = base_rule_->length();
        if (!old_length.has_omega_part()) {
            return ordinal_operations::add_finite(old_length, 1);
        }
        ordinal_number end_index(old_length.omega_coefficient(), old_length.finite_part());
        if (same_index(insertion_index_, end_index)) {
            return ordinal_operations::add_finite(old_length, 1);
        }
        if (insertion_index_.omega_coefficient() == old_length.omega_coefficient()) {
            return ordinal_operations::add_finite(old_length, 1);
        }
        return old_length;
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<inserted_element_rule<T>>(base_rule_, inserted_element_, insertion_index_);
    }
};


template<class T>
class concatenated_sequence_rule : public element_generation_rule<T> {
private:
    std::shared_ptr<element_generation_rule<T>> left_rule_;
    std::shared_ptr<element_generation_rule<T>> right_rule_;

public:
    concatenated_sequence_rule(std::shared_ptr<element_generation_rule<T>> left_rule,
                               std::shared_ptr<element_generation_rule<T>> right_rule)
        : left_rule_(std::move(left_rule)), right_rule_(std::move(right_rule)) {}

    T get(const ordinal_number& index) const override {
        ordinal_number left_length = left_rule_->length();
        if (ordinal_operations::contains(left_length, index)) {
            return left_rule_->get(index);
        }

        if (!left_length.has_omega_part()) {
            if (index.omega_coefficient() == 0) {
                return right_rule_->get(ordinal_number::finite(index.finite_part() - left_length.finite_part()));
            }
            return right_rule_->get(index);
        }

        std::size_t start_block = left_length.omega_coefficient();
        std::size_t start_offset = left_length.finite_part();
        if (index.omega_coefficient() < start_block ||
            (index.omega_coefficient() == start_block && index.finite_part() < start_offset)) {
            throw index_out_of_range_exception("lazy_sequence: index out of range");
        }
        if (index.omega_coefficient() == start_block) {
            return right_rule_->get(ordinal_number::finite(index.finite_part() - start_offset));
        }
        return right_rule_->get(ordinal_number(index.omega_coefficient() - start_block, index.finite_part()));
    }

    ordinal_number length() const override {
        return ordinal_operations::concat(left_rule_->length(), right_rule_->length());
    }

    std::shared_ptr<element_generation_rule<T>> clone_rule() const override {
        return std::make_shared<concatenated_sequence_rule<T>>(left_rule_, right_rule_);
    }
};

template<class From, class To>
class mapped_sequence_rule : public element_generation_rule<To> {
private:
    std::shared_ptr<element_generation_rule<From>> base_rule_;
    std::function<To(const From&)> transform_rule_;
public:
    mapped_sequence_rule(std::shared_ptr<element_generation_rule<From>> base_rule,
                         std::function<To(const From&)> transform_rule)
        : base_rule_(std::move(base_rule)), transform_rule_(std::move(transform_rule)) {}

    To get(const ordinal_number& index) const override {
        return transform_rule_(base_rule_->get(index));
    }

    ordinal_number length() const override {
        return base_rule_->length();
    }

    std::shared_ptr<element_generation_rule<To>> clone_rule() const override {
        return std::make_shared<mapped_sequence_rule<From, To>>(base_rule_, transform_rule_);
    }
};

#endif
