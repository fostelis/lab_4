#ifndef BIT_SEQUENCE_H
#define BIT_SEQUENCE_H

#include "sequence.h"
#include "bit.h"
#include "dynamic_array.h"

class bit_sequence : public sequence<bit> {
private:
    dynamic_array<bit>* items_;

public:
    bit_sequence() : items_(new dynamic_array<bit>()) {}
    explicit bit_sequence(int size) : items_(new dynamic_array<bit>(size)) {}
    bit_sequence(const bool* bits, int count) : items_(new dynamic_array<bit>()) {
        for (int i = 0; i < count; ++i) {
            items_->resize(items_->get_size() + 1);
            items_->set(items_->get_size() - 1, bit(bits[i]));
        }
    }
    bit_sequence(const bit_sequence& other) : items_(new dynamic_array<bit>(*other.items_)) {}
    ~bit_sequence() override {
        delete items_;
    }

    bit get_first() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(0);
    }
    bit get_last() const override {
        if (items_->get_size() == 0) {
            throw empty_collection_exception("empty");
        }
        return items_->get(items_->get_size() - 1);
    }
    bit get(int index) const override {
        return items_->get(index);
    }

    bit_sequence* get_subsequence(int start, int end) const override {
        if (start < 0 || end >= items_->get_size() || start > end) {
            throw index_out_of_range_exception("invalid indices");
        }
        bool* sub = new bool[end - start + 1];
        for (int i = start; i <= end; ++i) {
            sub[i - start] = items_->get(i).get_value();
        }
        bit_sequence* result = new bit_sequence(sub, end - start + 1);
        delete[] sub;
        return result;
    }

    int get_length() const override {
        return items_->get_size();
    }

    bit_sequence* append(const bit& item) override {
        items_->resize(items_->get_size() + 1);
        items_->set(items_->get_size() - 1, item);
        return this;
    }
    bit_sequence* prepend(const bit& item) override {
        return insert_at(item, 0);
    }
    bit_sequence* insert_at(const bit& item, int index) override {
        if (index < 0 || index > items_->get_size()) {
            throw index_out_of_range_exception("index out of range");
        }
        int old = items_->get_size();
        items_->resize(old + 1);
        for (int i = old; i > index; --i) {
            items_->set(i, items_->get(i - 1));
        }
        items_->set(index, item);
        return this;
    }

    sequence<bit>* concat(const sequence<bit>& other) override {
        bit_sequence* result = new bit_sequence(*this);
        for (int i = 0; i < other.get_length(); ++i) {
            result->append(other.get(i));
        }
        return result;
    }

    sequence<bit>* clone() const override {
        return new bit_sequence(*this);
    }

    bit_sequence* bit_and(const bit_sequence* other) const {
        if (items_->get_size() != other->items_->get_size()) {
            throw index_out_of_range_exception("size mismatch");
        }
        bit_sequence* result = new bit_sequence(items_->get_size());
        for (int i = 0; i < items_->get_size(); ++i) {
            result->items_->set(i, items_->get(i) & other->items_->get(i));
        }
        return result;
    }
    bit_sequence* bit_or(const bit_sequence* other) const {
        if (items_->get_size() != other->items_->get_size()) {
            throw index_out_of_range_exception("size mismatch");
        }
        bit_sequence* result = new bit_sequence(items_->get_size());
        for (int i = 0; i < items_->get_size(); ++i) {
            result->items_->set(i, items_->get(i) | other->items_->get(i));
        }
        return result;
    }
    bit_sequence* bit_xor(const bit_sequence* other) const {
        if (items_->get_size() != other->items_->get_size()) {
            throw index_out_of_range_exception("size mismatch");
        }
        bit_sequence* result = new bit_sequence(items_->get_size());
        for (int i = 0; i < items_->get_size(); ++i) {
            result->items_->set(i, items_->get(i) ^ other->items_->get(i));
        }
        return result;
    }
    bit_sequence* bit_not() const {
        bit_sequence* result = new bit_sequence(items_->get_size());
        for (int i = 0; i < items_->get_size(); ++i) {
            result->items_->set(i, !items_->get(i));
        }
        return result;
    }
};

#endif
