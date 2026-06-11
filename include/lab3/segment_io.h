#ifndef SEGMENT_IO_H
#define SEGMENT_IO_H

#include <iostream>
#include "segment.h"


inline std::ostream& operator<<(std::ostream& os, const segment& seg) {
    os << "[" << seg.start_x << ", " << seg.end_x << "]: y = "
       << seg.k << "x + " << seg.m;
    return os;
}

#endif
