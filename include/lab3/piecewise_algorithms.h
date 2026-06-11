#ifndef PIECEWISE_ALGORITHMS_H
#define PIECEWISE_ALGORITHMS_H

#include "piecewise_function.h"
#include "mutable_piecewise_function.h"


template<class Func>
mutable_piecewise_function* map_segments(const piecewise_function* pf, Func func) {
    sequence<segment>* new_segs = new mutable_array_sequence<segment>();
    for (int i = 0; i < pf->get_segment_count(); i++) {
        new_segs = new_segs->append(func(pf->get_segment(i)));
    }
    return new mutable_piecewise_function(new_segs);
}

template<class Pred>
mutable_piecewise_function* filter_segments(const piecewise_function* pf, Pred pred) {
    sequence<segment>* new_segs = new mutable_array_sequence<segment>();
    for (int i = 0; i < pf->get_segment_count(); i++) {
        if (pred(pf->get_segment(i))) {
            new_segs = new_segs->append(pf->get_segment(i));
        }
    }
    return new mutable_piecewise_function(new_segs);
}

template<class Acc, class Func>
Acc reduce_segments(const piecewise_function* pf, Func func, Acc initial) {
    Acc res = initial;
    for (int i = 0; i < pf->get_segment_count(); i++) {
        res = func(res, pf->get_segment(i));
    }
    return res;
}

#endif
