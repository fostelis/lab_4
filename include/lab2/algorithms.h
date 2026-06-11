#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "sequence.h"

template<class T>
struct stats {
    T min_val;
    T max_val;
    double avg_val;
};

template<class T>
stats<T> get_stats(const sequence<T>& seq) {
    stats<T> result{};
    if (seq.get_length() == 0) {
        return result;
    }
    result.min_val = seq.get(0);
    result.max_val = seq.get(0);
    double sum = 0;
    for (int i = 0; i < seq.get_length(); ++i) {
        T val = seq.get(i);
        if (val < result.min_val) {
            result.min_val = val;
        }
        if (val > result.max_val) {
            result.max_val = val;
        }
        sum += static_cast<double>(val);
    }
    result.avg_val = sum / seq.get_length();
    return result;
}

template<class T>
sequence<sequence<T>*>* get_all_prefixes(const sequence<T>& seq) {
    sequence<sequence<T>*>* result = new mutable_array_sequence<sequence<T>*>();
    for (int i = 1; i <= seq.get_length(); ++i) {
        sequence<T>* prefix = new mutable_array_sequence<T>();
        for (int j = 0; j < i; ++j) {
            sequence<T>* new_prefix = prefix->append(seq.get(j));
            if (new_prefix != prefix) {
                delete prefix;
            }
            prefix = new_prefix;
        }
        result = result->append(prefix);
    }
    return result;
}

template<class T>
sequence<double>* moving_average_3(const sequence<T>& seq) {
    sequence<double>* result = new mutable_array_sequence<double>();
    for (int i = 0; i < seq.get_length(); ++i) {
        double sum = seq.get(i);
        int count = 1;
        if (i > 0) {
            sum += seq.get(i - 1);
            ++count;
        }
        if (i < seq.get_length() - 1) {
            sum += seq.get(i + 1);
            ++count;
        }
        result = result->append(sum / count);
    }
    return result;
}

template<class T>
sequence<T>* sum_with_reflection(const sequence<T>& seq) {
    sequence<T>* result = new mutable_array_sequence<T>();
    int n = seq.get_length();
    for (int i = 0; i < n; ++i) {
        result = result->append(seq.get(i) + seq.get(n - 1 - i));
    }
    return result;
}

template<class T>
int count_inversions(const sequence<T>& seq) {
    int count = 0;
    for (int i = 0; i < seq.get_length(); ++i) {
        for (int j = i + 1; j < seq.get_length(); ++j) {
            if (seq.get(i) > seq.get(j)) {
                ++count;
            }
        }
    }
    return count;
}

template<class T>
void delete_split_result(sequence<sequence<T>*>* split_result) {
    if (!split_result) {
        return;
    }
    for (int i = 0; i < split_result->get_length(); ++i) {
        delete split_result->get(i);
    }
    delete split_result;
}

#endif
