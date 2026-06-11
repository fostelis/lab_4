#ifndef SEGMENT_H
#define SEGMENT_H

struct segment {
    double start_x;
    double end_x;
    double k;
    double m;

    segment() : start_x(0), end_x(0), k(0), m(0) {}
    segment(double s, double e, double k_val, double m_val)
        : start_x(s), end_x(e), k(k_val), m(m_val) {}

    double evaluate(double x) const {
        return k * x + m;
    }
};

#endif
