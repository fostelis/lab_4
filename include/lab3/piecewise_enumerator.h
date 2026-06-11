#ifndef PIECEWISE_ENUMERATOR_H
#define PIECEWISE_ENUMERATOR_H

#include "segment.h"
#include "sequence.h"

class piecewise_enumerator {
private:
    const sequence<segment>& segments_ref_;
    int index_;

public:

    explicit piecewise_enumerator(const sequence<segment>& segs)
        : segments_ref_(segs), index_(-1) {}

    bool move_next() {
        index_++;
        return index_ < segments_ref_.get_length();
    }

    segment current() const {
        if (index_ < 0 || index_ >= segments_ref_.get_length()) {
            throw index_out_of_range_exception("enumerator out of range");
        }
        return segments_ref_.get(index_);
    }

    void reset() {
        index_ = -1;
    }
};

#endif
