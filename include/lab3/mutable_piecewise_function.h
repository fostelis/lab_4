#ifndef MUTABLE_PIECEWISE_FUNCTION_H
#define MUTABLE_PIECEWISE_FUNCTION_H

#include "piecewise_function.h"


class mutable_piecewise_function : public piecewise_function {
public:
    mutable_piecewise_function() : piecewise_function() {}
    mutable_piecewise_function(sequence<segment>* segs) : piecewise_function(segs) {}
    mutable_piecewise_function(const mutable_piecewise_function& other) : piecewise_function(other) {}

    piecewise_function* redefine_on_interval(double start, double end, double k, double m) override {
        sequence<segment>* new_segs = new mutable_array_sequence<segment>();
        for (int i = 0; i < segments_->get_length(); ++i) {
            segment seg = segments_->get(i);
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
        delete segments_;
        segments_ = new_segs;
        return this;
    }

    piecewise_function* add_segment(double start, double end, double k, double m) override {
        segments_ = segments_->append(segment(start, end, k, m));
        return this;
    }

    piecewise_function* remove_segment(int index) override {
        if (!segments_ || index < 0 || index >= segments_->get_length()) {
            throw index_out_of_range_exception("remove_segment: index out of range");
        }
        sequence<segment>* new_segs = new mutable_array_sequence<segment>();
        for (int i = 0; i < segments_->get_length(); ++i) {
            if (i != index) {
                new_segs = new_segs->append(segments_->get(i));
            }
        }
        delete segments_;
        segments_ = new_segs;
        return this;
    }

    piecewise_function* concat(const piecewise_function& other) override {
        for (int i = 0; i < other.get_segment_count(); ++i) {
            segments_ = segments_->append(other.get_segment(i));
        }
        return this;
    }

    piecewise_function* clone() const override {
        return new mutable_piecewise_function(*this);
    }

protected:
    piecewise_function* instance() override {
        return this;
    }
};

#endif
