#ifndef ONLINE_STATISTICS_H
#define ONLINE_STATISTICS_H

#include <queue>
#include <vector>
#include "streams.h"

struct online_statistics_snapshot {
    std::size_t count = 0;
    double min = 0;
    double max = 0;
    double mean = 0;
    double median = 0;
};

class online_statistics_collector {
private:
    std::priority_queue<double> lower_;
    std::priority_queue<double, std::vector<double>, std::greater<double>> upper_;
    std::size_t count_ = 0;
    double sum_ = 0;
    double min_ = 0;
    double max_ = 0;

    void rebalance() {
        if (lower_.size() > upper_.size() + 1) {
            upper_.push(lower_.top());
            lower_.pop();
        } else if (upper_.size() > lower_.size()) {
            lower_.push(upper_.top());
            upper_.pop();
        }
    }

public:
    void add(double value) {
        if (count_ == 0) {
            min_ = max_ = value;
        } else {
            if (value < min_) min_ = value;
            if (value > max_) max_ = value;
        }
        ++count_;
        sum_ += value;

        if (lower_.empty() || value <= lower_.top()) lower_.push(value);
        else upper_.push(value);
        rebalance();
    }

    online_statistics_snapshot snapshot() const {
        online_statistics_snapshot s;
        s.count = count_;
        if (count_ == 0) return s;
        s.min = min_;
        s.max = max_;
        s.mean = sum_ / static_cast<double>(count_);
        if (lower_.size() == upper_.size()) s.median = (lower_.top() + upper_.top()) / 2.0;
        else s.median = lower_.top();
        return s;
    }
};

inline online_statistics_snapshot collect_statistics(read_only_stream<double>& stream) {
    online_statistics_collector collector;
    stream.open();
    while (!stream.is_end_of_stream()) collector.add(stream.read());
    stream.close();
    return collector.snapshot();
}

#endif
