#ifndef INTERVAL_H
#define INTERVAL_H


struct interval {
    double left;
    double right;

    interval() : left(0), right(0) {}
    interval(double l, double r) : left(l), right(r) {}


    bool contains(double x) const {
        return x >= left && x <= right;
    }


    bool overlaps(const interval& other) const {
        return !(right < other.left || left > other.right);
    }


    double length() const {
        return right - left;
    }
};

#endif
