#ifndef IMMUTABLE_PIECEWISE_FUNCTION_H
#define IMMUTABLE_PIECEWISE_FUNCTION_H

#include "piecewise_function.h"


class immutable_piecewise_function : public piecewise_function {
public:
    immutable_piecewise_function() : piecewise_function() {}
    immutable_piecewise_function(sequence<segment>* segs) : piecewise_function(segs) {}
    immutable_piecewise_function(const immutable_piecewise_function& other) : piecewise_function(other) {}

    piecewise_function* redefine_on_interval(double start, double end, double k, double m) override {
        immutable_piecewise_function* copy = new immutable_piecewise_function(*this);
        sequence<segment>* new_segs = new mutable_array_sequence<segment>();
        for (int i = 0; i < copy->segments_->get_length(); ++i) {
            segment seg = copy->segments_->get(i);
            if (seg.end_x <= start || seg.start_x >= end) {
                new_segs = new_segs->append(seg);
            }
            else {
                if (seg.start_x < start) {
                    new_segs = new_segs->append(segment(seg.start_x, start, seg.k, seg.m));
                }
                new_segs = new_segs->append(segment(std::max(seg.start_x, start), std::min(seg.end_x, end), k, m));
                if (seg.end_x > end) {
                    new_segs = new_segs->append(segment(end, seg.end_x, seg.k, seg.m));
                }
            }
        }
        delete copy->segments_;
        copy->segments_ = new_segs;
        return copy;
    }

    piecewise_function* add_segment(double start, double end, double k, double m) override {
        immutable_piecewise_function* copy = new immutable_piecewise_function(*this);
        copy->segments_ = copy->segments_->append(segment(start, end, k, m));
        return copy;
    }

    piecewise_function* remove_segment(int index) override {
        if (!this->segments_ || index < 0 || index >= this->segments_->get_length()) {
            throw index_out_of_range_exception("remove_segment: index out of range");
        }
        immutable_piecewise_function* copy = new immutable_piecewise_function(*this);
        sequence<segment>* new_segs = new mutable_array_sequence<segment>();
        for (int i = 0; i < copy->segments_->get_length(); ++i) {
            if (i != index) {
                new_segs = new_segs->append(copy->segments_->get(i));
            }
        }
        delete copy->segments_;
        copy->segments_ = new_segs;
        return copy;
    }

    piecewise_function* concat(const piecewise_function& other) override {
        immutable_piecewise_function* copy = new immutable_piecewise_function(*this);
        for (int i = 0; i < other.get_segment_count(); ++i) {
            copy->segments_ = copy->segments_->append(other.get_segment(i));
        }
        return copy;
    }

    piecewise_function* clone() const override {
        return new immutable_piecewise_function(*this);
    }

protected:
    piecewise_function* instance() override {
        return new immutable_piecewise_function(*this);
    }
};

#endif
