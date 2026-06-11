#ifndef PIECEWISE_FUNCTION_H
#define PIECEWISE_FUNCTION_H

#include "sequence.h"
#include "segment.h"
#include "array_sequence.h"
#include "interval.h"
#include <cmath>


class piecewise_function {
protected:
    sequence<segment>* segments_;
    virtual piecewise_function* instance() = 0;

public:
    piecewise_function() : segments_(new mutable_array_sequence<segment>()) {}
    piecewise_function(sequence<segment>* segs) : segments_(segs) {}
    piecewise_function(const piecewise_function& other) : segments_(other.segments_ ? other.segments_->clone() : nullptr) {}
    virtual ~piecewise_function() {
        delete segments_;
    }

    double evaluate(double x) const {
        for (int i = 0; i < segments_->get_length(); ++i) {
            segment seg = segments_->get(i);
            bool is_last = (i == segments_->get_length() - 1);
            if (x >= seg.start_x && (x < seg.end_x || (is_last && x <= seg.end_x))) {
                return seg.evaluate(x);
            }
        }
        throw index_out_of_range_exception("x out of function domain");
    }

    bool is_continuous_on(const interval& ivl, double epsilon = 1e-9) const {
        if (segments_->get_length() <= 1) {
            return true;
        }
        int first = -1;
        int last = -1;
        for (int i = 0; i < segments_->get_length(); ++i) {
            segment seg = segments_->get(i);
            if (seg.start_x < ivl.right && seg.end_x > ivl.left) {
                if (first == -1) {
                    first = i;
                }
                last = i;
            }
        }
        if (first == -1 || first == last) {
            return true;
        }

        for (int i = first; i < last; ++i) {
            segment curr = segments_->get(i);
            segment next = segments_->get(i + 1);
            double stitch = curr.end_x;

            if (stitch <= ivl.left || stitch >= ivl.right) {
                continue;
            }

            if (std::abs(curr.evaluate(stitch) - next.evaluate(stitch)) > epsilon) {
                return false;
            }
        }
        return true;
    }

    bool is_monotonic_on(const interval& ivl) const {
        if (segments_->get_length() <= 1) {
            return true;
        }

        bool inc = true;
        bool dec = true;

        for (int i = 0; i < segments_->get_length(); ++i) {
            segment seg = segments_->get(i);

            double left = std::max(seg.start_x, ivl.left);
            double right = std::min(seg.end_x, ivl.right);

            if (left >= right) {
                continue;
            }

            if (seg.k < 0) {
                inc = false;
            }
            if (seg.k > 0) {
                dec = false;
            }
        }

        for (int i = 0; i < segments_->get_length() - 1; ++i) {
            segment curr = segments_->get(i);
            segment next = segments_->get(i + 1);
            double stitch = curr.end_x;

            if (stitch <= ivl.left || stitch >= ivl.right) {
                continue;
            }

            double left_val = curr.evaluate(stitch);
            double right_val = next.evaluate(stitch);

            if (left_val > right_val) {
                inc = false;
            }
            if (left_val < right_val) {
                dec = false;
            }
        }

        return inc || dec;
    }


    bool is_continuous(double epsilon = 1e-9) const {
        if (segments_->get_length() == 0) {
            return true;
        }
        if (segments_->get_length() == 1) {
            return true;
        }
        interval full(segments_->get(0).start_x,
                      segments_->get(segments_->get_length() - 1).end_x);
        return is_continuous_on(full, epsilon);
    }

    bool is_monotonic() const {
        if (segments_->get_length() == 0) {
            return true;
        }
        if (segments_->get_length() == 1) {
            return true;
        }
        interval full(segments_->get(0).start_x,
                      segments_->get(segments_->get_length() - 1).end_x);
        return is_monotonic_on(full);
    }

    virtual piecewise_function* redefine_on_interval(double start, double end, double k, double m) = 0;
    virtual piecewise_function* add_segment(double start, double end, double k, double m) = 0;
    virtual piecewise_function* remove_segment(int index) = 0;
    virtual piecewise_function* concat(const piecewise_function& other) = 0;

    int get_segment_count() const {
        return segments_ ? segments_->get_length() : 0;
    }
    segment get_segment(int index) const {
        return segments_->get(index);
    }
    sequence<segment>* get_segments() const {
        return segments_;
    }

    virtual piecewise_function* clone() const = 0;
};

#endif
