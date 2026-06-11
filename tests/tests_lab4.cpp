#include <cmath>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "array_sequence.h"
#include "exceptions.h"
#include "generation_window.h"
#include "lazy_sequence.h"
#include "online_statistics.h"
#include "ordinal_operations.h"
#include "streams.h"
#include "lab4_format.h"

static bool approx4(double left, double right, double eps = 1e-9) {
    return std::abs(left - right) < eps;
}

static int fibonacci_int_rule(const sequence<int>& values, std::size_t) {
    return values.get(values.get_length() - 1) + values.get(values.get_length() - 2);
}

static int natural_int_rule(const sequence<int>&, std::size_t index) {
    return static_cast<int>(index);
}

static int thousand_natural_int_rule(const sequence<int>&, std::size_t index) {
    return static_cast<int>(1000 + index);
}

static int negative_natural_int_rule(const sequence<int>&, std::size_t index) {
    return -static_cast<int>(index);
}

static int constant_seven_rule(const sequence<int>&, std::size_t) {
    return 7;
}

static int multiply_by_two_int(const int& value) {
    return value * 2;
}

static int square_int_value(const int& value) {
    return value * value;
}

static bool is_even_int(const int& value) {
    return value % 2 == 0;
}

static bool is_positive_int(const int& value) {
    return value > 0;
}

static int sum_int_values(const int& acc, const int& value) {
    return acc + value;
}

static int product_int_values(const int& acc, const int& value) {
    return acc * value;
}

static int parse_int_line(const std::string& line) {
    return std::stoi(line);
}

static double parse_double_line(const std::string& line) {
    return std::stod(line);
}

static std::string int_to_line(const int& value) {
    return std::to_string(value);
}

static std::string double_to_line(const double& value) {
    return std::to_string(value);
}

static lazy_sequence<int> make_fibonacci() {
    int seed_items[] = {1, 1};
    mutable_array_sequence<int> seed(seed_items, 2);
    return lazy_sequence<int>(fibonacci_int_rule, seed);
}

static lazy_sequence<int> make_naturals() {
    int seed_items[] = {0};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(natural_int_rule, seed);
}

static lazy_sequence<int> make_thousand_naturals() {
    int seed_items[] = {1000};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(thousand_natural_int_rule, seed);
}

static lazy_sequence<int> make_negative_naturals() {
    int seed_items[] = {0};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(negative_natural_int_rule, seed);
}

static lazy_sequence<int> make_constant_sevens() {
    int seed_items[] = {7};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(constant_seven_rule, seed);
}

TEST(Lab4OrdinalNumber, FiniteFactory) {
    ordinal_number value = ordinal_number::finite(15);
    EXPECT_TRUE(value.is_finite());
    EXPECT_FALSE(value.has_omega_part());
    EXPECT_EQ(value.finite_value(), static_cast<std::size_t>(15));
    EXPECT_EQ(value.finite_part(), static_cast<std::size_t>(15));
    EXPECT_EQ(value.omega_coefficient(), static_cast<std::size_t>(0));
}

TEST(Lab4OrdinalNumber, OmegaFactory) {
    ordinal_number value = ordinal_number::omega(3, 7);
    EXPECT_FALSE(value.is_finite());
    EXPECT_TRUE(value.has_omega_part());
    EXPECT_EQ(value.omega_coefficient(), static_cast<std::size_t>(3));
    EXPECT_EQ(value.finite_part(), static_cast<std::size_t>(7));
    EXPECT_THROW(value.finite_value(), index_out_of_range_exception);
}

TEST(Lab4OrdinalNumber, Equality) {
    ordinal_number first = ordinal_number::omega(2, 5);
    ordinal_number second = ordinal_number::omega(2, 5);
    ordinal_number third = ordinal_number::omega(2, 6);
    EXPECT_TRUE(first.equals(second));
    EXPECT_FALSE(first.equals(third));
}

TEST(Lab4OrdinalOperations, ContainsFiniteLength) {
    ordinal_number length = ordinal_number::finite(3);
    EXPECT_TRUE(ordinal_operations::contains(length, ordinal_number::finite(0)));
    EXPECT_TRUE(ordinal_operations::contains(length, ordinal_number::finite(2)));
    EXPECT_FALSE(ordinal_operations::contains(length, ordinal_number::finite(3)));
    EXPECT_FALSE(ordinal_operations::contains(length, ordinal_number::omega(1, 0)));
}

TEST(Lab4OrdinalOperations, ContainsOmegaLength) {
    ordinal_number length = ordinal_number::omega(2, 3);
    EXPECT_TRUE(ordinal_operations::contains(length, ordinal_number::omega(0, 100)));
    EXPECT_TRUE(ordinal_operations::contains(length, ordinal_number::omega(1, 100)));
    EXPECT_TRUE(ordinal_operations::contains(length, ordinal_number::omega(2, 2)));
    EXPECT_FALSE(ordinal_operations::contains(length, ordinal_number::omega(2, 3)));
    EXPECT_FALSE(ordinal_operations::contains(length, ordinal_number::omega(3, 0)));
}

TEST(Lab4OrdinalOperations, CanInsertAtFiniteLength) {
    ordinal_number length = ordinal_number::finite(3);
    EXPECT_TRUE(ordinal_operations::can_insert_at(length, ordinal_number::finite(0)));
    EXPECT_TRUE(ordinal_operations::can_insert_at(length, ordinal_number::finite(3)));
    EXPECT_FALSE(ordinal_operations::can_insert_at(length, ordinal_number::finite(4)));
    EXPECT_FALSE(ordinal_operations::can_insert_at(length, ordinal_number::omega(1, 0)));
}

TEST(Lab4OrdinalOperations, CanInsertAtOmegaTail) {
    ordinal_number length = ordinal_number::omega(2, 4);
    EXPECT_TRUE(ordinal_operations::can_insert_at(length, ordinal_number::omega(0, 10)));
    EXPECT_TRUE(ordinal_operations::can_insert_at(length, ordinal_number::omega(2, 4)));
    EXPECT_FALSE(ordinal_operations::can_insert_at(length, ordinal_number::omega(2, 5)));
}

TEST(Lab4OrdinalOperations, LastIndex) {
    EXPECT_FALSE(ordinal_operations::has_last(ordinal_number::omega(3, 0)));
    EXPECT_FALSE(ordinal_operations::has_last(ordinal_number::finite(0)));
    EXPECT_TRUE(ordinal_operations::has_last(ordinal_number::finite(4)));
    EXPECT_TRUE(ordinal_operations::has_last(ordinal_number::omega(3, 2)));
    EXPECT_TRUE(ordinal_operations::last_index(ordinal_number::finite(4)).equals(ordinal_number::finite(3)));
    EXPECT_TRUE(ordinal_operations::last_index(ordinal_number::omega(3, 2)).equals(ordinal_number::omega(3, 1)));
    EXPECT_THROW(ordinal_operations::last_index(ordinal_number::omega(1, 0)), index_out_of_range_exception);
}

TEST(Lab4OrdinalOperations, AddFiniteAndConcat) {
    EXPECT_TRUE(ordinal_operations::add_finite(ordinal_number::finite(2), 3).equals(ordinal_number::finite(5)));
    EXPECT_TRUE(ordinal_operations::add_finite(ordinal_number::omega(1, 2), 3).equals(ordinal_number::omega(1, 5)));
    EXPECT_TRUE(ordinal_operations::concat(ordinal_number::finite(2), ordinal_number::finite(3)).equals(ordinal_number::finite(5)));
    EXPECT_TRUE(ordinal_operations::concat(ordinal_number::omega(1, 0), ordinal_number::finite(2)).equals(ordinal_number::omega(1, 2)));
    EXPECT_TRUE(ordinal_operations::concat(ordinal_number::omega(2, 1), ordinal_number::omega(3, 4)).equals(ordinal_number::omega(5, 4)));
}

TEST(Lab4GenerationWindow, EmptyWindowProperties) {
    generation_window<int> window(3);
    EXPECT_EQ(window.size(), static_cast<std::size_t>(0));
    EXPECT_EQ(window.capacity(), static_cast<std::size_t>(3));
    EXPECT_EQ(window.first_index(), static_cast<std::size_t>(0));
    EXPECT_FALSE(window.contains(0));
    EXPECT_THROW(window.last_index(), empty_collection_exception);
}

TEST(Lab4GenerationWindow, PushWithoutOverflow) {
    generation_window<int> window(3);
    window.push_back(10);
    window.push_back(20);
    EXPECT_TRUE(window.contains(0));
    EXPECT_TRUE(window.contains(1));
    EXPECT_EQ(window.get(0), 10);
    EXPECT_EQ(window.get(1), 20);
    EXPECT_EQ(window.last_index(), static_cast<std::size_t>(1));
}

TEST(Lab4GenerationWindow, RingBufferOverflow) {
    generation_window<int> window(3);
    window.push_back(10);
    window.push_back(20);
    window.push_back(30);
    window.push_back(40);
    EXPECT_FALSE(window.contains(0));
    EXPECT_TRUE(window.contains(1));
    EXPECT_TRUE(window.contains(3));
    EXPECT_EQ(window.first_index(), static_cast<std::size_t>(1));
    EXPECT_EQ(window.get(1), 20);
    EXPECT_EQ(window.get(3), 40);
    EXPECT_THROW(window.get(0), index_out_of_range_exception);
}

TEST(Lab4GenerationWindow, ZeroCapacityBecomesOne) {
    generation_window<int> window(0);
    window.push_back(1);
    window.push_back(2);
    EXPECT_EQ(window.capacity(), static_cast<std::size_t>(1));
    EXPECT_EQ(window.size(), static_cast<std::size_t>(1));
    EXPECT_FALSE(window.contains(0));
    EXPECT_TRUE(window.contains(1));
    EXPECT_EQ(window.get(1), 2);
}

TEST(Lab4LazySequenceConstructors, EmptySequence) {
    lazy_sequence<int> sequence;
    EXPECT_TRUE(sequence.get_length().equals(ordinal_number::finite(0)));
    EXPECT_EQ(sequence.get_materialized_count(), static_cast<std::size_t>(0));
    EXPECT_THROW(sequence.get_first(), index_out_of_range_exception);
    EXPECT_THROW(sequence.get_last(), index_out_of_range_exception);
}

TEST(Lab4LazySequenceConstructors, ArraySequence) {
    int data[] = {4, 5, 6};
    lazy_sequence<int> sequence(data, 3);
    EXPECT_TRUE(sequence.get_length().equals(ordinal_number::finite(3)));
    EXPECT_EQ(sequence.get_materialized_count(), static_cast<std::size_t>(3));
    EXPECT_EQ(sequence.get_first(), 4);
    EXPECT_EQ(sequence.get_last(), 6);
    EXPECT_EQ(sequence.get(1), 5);
}

TEST(Lab4LazySequenceConstructors, ArrayErrors) {
    int data[] = {1};
    EXPECT_THROW(lazy_sequence<int> bad(data, -1), index_out_of_range_exception);
    EXPECT_THROW(lazy_sequence<int> bad(nullptr, 2), index_out_of_range_exception);
}

TEST(Lab4LazySequenceConstructors, FromSequence) {
    int data[] = {8, 9, 10};
    mutable_array_sequence<int> source(data, 3);
    lazy_sequence<int> sequence(source);
    EXPECT_TRUE(sequence.get_length().equals(ordinal_number::finite(3)));
    EXPECT_EQ(sequence.get(0), 8);
    EXPECT_EQ(sequence.get(2), 10);
}

TEST(Lab4LazySequenceConstructors, NegativeIndexThrows) {
    int data[] = {1, 2};
    lazy_sequence<int> sequence(data, 2);
    EXPECT_THROW(sequence.get(-1), index_out_of_range_exception);
    EXPECT_THROW(sequence.get(2), index_out_of_range_exception);
}

TEST(Lab4LazySequenceRecurrent, FibonacciBasic) {
    lazy_sequence<int> fib = make_fibonacci();
    EXPECT_TRUE(fib.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(fib.get_materialized_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(fib.get(0), 1);
    EXPECT_EQ(fib.get(1), 1);
    EXPECT_EQ(fib.get(5), 8);
    EXPECT_EQ(fib.get_materialized_count(), static_cast<std::size_t>(6));
}

TEST(Lab4LazySequenceRecurrent, FibonacciTake) {
    lazy_sequence<int> fib = make_fibonacci();
    mutable_array_sequence<int> first = fib.take(7);
    EXPECT_EQ(first.get_length(), 7);
    EXPECT_EQ(first.get(0), 1);
    EXPECT_EQ(first.get(6), 13);
}

TEST(Lab4LazySequenceRecurrent, InfiniteHasNoLast) {
    lazy_sequence<int> fib = make_fibonacci();
    EXPECT_THROW(fib.get_last(), index_out_of_range_exception);
}

TEST(Lab4LazySequenceRecurrent, FiniteRecurrentLength) {
    int seed_items[] = {0};
    mutable_array_sequence<int> seed(seed_items, 1);
    lazy_sequence<int> sequence(natural_int_rule, seed, 5);
    EXPECT_TRUE(sequence.get_length().equals(ordinal_number::finite(5)));
    EXPECT_EQ(sequence.get(4), 4);
    EXPECT_THROW(sequence.get(5), index_out_of_range_exception);
    EXPECT_EQ(sequence.get_last(), 4);
}

TEST(Lab4LazySequenceRecurrent, WindowKeepsLimitedMemory) {
    int seed_items[] = {0};
    mutable_array_sequence<int> seed(seed_items, 1);
    lazy_sequence<int> sequence(natural_int_rule, seed, std::nullopt, 5);
    EXPECT_EQ(sequence.get(9), 9);
    EXPECT_TRUE(sequence.get_materialized_count() <= 5);
    EXPECT_EQ(sequence.get(7), 7);
    EXPECT_THROW(sequence.get(0), index_out_of_range_exception);
}

TEST(Lab4LazySequenceModifiers, Subsequence) {
    int data[] = {10, 20, 30, 40};
    lazy_sequence<int> sequence(data, 4);
    lazy_sequence<int> sub = sequence.get_subsequence(1, 2);
    EXPECT_TRUE(sub.get_length().equals(ordinal_number::finite(2)));
    EXPECT_EQ(sub.get(0), 20);
    EXPECT_EQ(sub.get(1), 30);
    EXPECT_THROW(sequence.get_subsequence(2, 1), index_out_of_range_exception);
}

TEST(Lab4LazySequenceModifiers, AppendFinite) {
    int data[] = {1, 2};
    lazy_sequence<int> sequence(data, 2);
    lazy_sequence<int> result = sequence.append(3);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(3)));
    EXPECT_EQ(result.get_last(), 3);
    EXPECT_TRUE(sequence.get_length().equals(ordinal_number::finite(2)));
}

TEST(Lab4LazySequenceModifiers, PrependFinite) {
    int data[] = {1, 2};
    lazy_sequence<int> sequence(data, 2);
    lazy_sequence<int> result = sequence.prepend(0);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(3)));
    EXPECT_EQ(result.get(0), 0);
    EXPECT_EQ(result.get(1), 1);
}

TEST(Lab4LazySequenceModifiers, InsertFiniteMiddle) {
    int data[] = {1, 2, 4};
    lazy_sequence<int> sequence(data, 3);
    lazy_sequence<int> result = sequence.insert_at(3, 2);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(4)));
    EXPECT_EQ(result.get(0), 1);
    EXPECT_EQ(result.get(2), 3);
    EXPECT_EQ(result.get(3), 4);
}

TEST(Lab4LazySequenceModifiers, InsertFiniteEdges) {
    int data[] = {1, 2};
    lazy_sequence<int> sequence(data, 2);
    EXPECT_EQ(sequence.insert_at(0, 0).get_first(), 0);
    EXPECT_EQ(sequence.insert_at(3, 2).get_last(), 3);
    EXPECT_THROW(sequence.insert_at(4, 3), index_out_of_range_exception);
    EXPECT_THROW(sequence.insert_at(4, -1), index_out_of_range_exception);
}

TEST(Lab4LazySequenceModifiers, PrependInfinite) {
    lazy_sequence<int> sequence = make_naturals();
    lazy_sequence<int> result = sequence.prepend(-1);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(result.get(0), -1);
    EXPECT_EQ(result.get(1), 0);
    EXPECT_EQ(result.get(4), 3);
}

TEST(Lab4LazySequenceModifiers, AppendInfiniteCreatesFiniteTail) {
    lazy_sequence<int> sequence = make_naturals();
    lazy_sequence<int> result = sequence.append(777).append(888);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega(1, 2)));
    EXPECT_EQ(result.get(ordinal_number::omega(1, 0)), 777);
    EXPECT_EQ(result.get(ordinal_number::omega(1, 1)), 888);
    EXPECT_EQ(result.get_last(), 888);
}

TEST(Lab4LazySequenceModifiers, InsertInsideInfinite) {
    lazy_sequence<int> sequence = make_naturals();
    lazy_sequence<int> result = sequence.insert_at(42, ordinal_number::finite(5));
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(result.get(4), 4);
    EXPECT_EQ(result.get(5), 42);
    EXPECT_EQ(result.get(6), 5);
}

TEST(Lab4LazySequenceModifiers, InsertAtOmegaTail) {
    lazy_sequence<int> sequence = make_naturals();
    lazy_sequence<int> result = sequence.insert_at(555, ordinal_number::omega(1, 0));
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega(1, 1)));
    EXPECT_EQ(result.get(ordinal_number::omega(1, 0)), 555);
    EXPECT_EQ(result.get_last(), 555);
}

TEST(Lab4LazySequenceConcat, FiniteConcatFinite) {
    int left_data[] = {1, 2};
    int right_data[] = {3, 4};
    lazy_sequence<int> left(left_data, 2);
    lazy_sequence<int> right(right_data, 2);
    lazy_sequence<int> result = left.concat(right);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(4)));
    EXPECT_EQ(result.get(0), 1);
    EXPECT_EQ(result.get(2), 3);
    EXPECT_EQ(result.get_last(), 4);
}

TEST(Lab4LazySequenceConcat, FiniteConcatInfinite) {
    int prefix_data[] = {9, 8};
    lazy_sequence<int> prefix(prefix_data, 2);
    lazy_sequence<int> naturals = make_naturals();
    lazy_sequence<int> result = prefix.concat(naturals);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(result.get(0), 9);
    EXPECT_EQ(result.get(1), 8);
    EXPECT_EQ(result.get(2), 0);
    EXPECT_EQ(result.get(6), 4);
}

TEST(Lab4LazySequenceConcat, InfiniteConcatFinite) {
    int tail_data[] = {100, 200};
    lazy_sequence<int> naturals = make_naturals();
    lazy_sequence<int> tail(tail_data, 2);
    lazy_sequence<int> result = naturals.concat(tail);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega(1, 2)));
    EXPECT_EQ(result.get(10), 10);
    EXPECT_EQ(result.get(ordinal_number::omega(1, 0)), 100);
    EXPECT_EQ(result.get(ordinal_number::omega(1, 1)), 200);
}

TEST(Lab4LazySequenceConcat, InfiniteConcatInfinite) {
    lazy_sequence<int> first = make_naturals();
    lazy_sequence<int> second = make_thousand_naturals();
    lazy_sequence<int> result = first.concat(second);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega(2, 0)));
    EXPECT_EQ(result.get(ordinal_number::omega(0, 7)), 7);
    EXPECT_EQ(result.get(ordinal_number::omega(1, 7)), 1007);
}

TEST(Lab4LazySequenceConcat, ThreeInfiniteSequences) {
    lazy_sequence<int> first = make_naturals();
    lazy_sequence<int> second = make_thousand_naturals();
    lazy_sequence<int> third = make_negative_naturals();
    lazy_sequence<int> result = first.concat(second).concat(third);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega(3, 0)));
    EXPECT_EQ(result.get(ordinal_number::omega(0, 3)), 3);
    EXPECT_EQ(result.get(ordinal_number::omega(1, 3)), 1003);
    EXPECT_EQ(result.get(ordinal_number::omega(2, 3)), -3);
}

TEST(Lab4LazySequenceFunctional, MapFinite) {
    int data[] = {1, 2, 3};
    lazy_sequence<int> sequence(data, 3);
    lazy_sequence<int> result = sequence.map<int>(multiply_by_two_int);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(3)));
    EXPECT_EQ(result.get(0), 2);
    EXPECT_EQ(result.get(2), 6);
}

TEST(Lab4LazySequenceFunctional, MapInfinite) {
    lazy_sequence<int> sequence = make_naturals();
    lazy_sequence<int> result = sequence.map<int>(square_int_value);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(result.get(5), 25);
    EXPECT_EQ(result.get(12), 144);
}

TEST(Lab4LazySequenceFunctional, WhereFinite) {
    int data[] = {1, 2, 3, 4, 5};
    lazy_sequence<int> sequence(data, 5);
    lazy_sequence<int> result = sequence.where(is_even_int);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(2)));
    EXPECT_EQ(result.get(0), 2);
    EXPECT_EQ(result.get(1), 4);
}

TEST(Lab4LazySequenceFunctional, WhereInfiniteLimited) {
    lazy_sequence<int> sequence = make_fibonacci();
    lazy_sequence<int> result = sequence.where(is_even_int, 8);
    EXPECT_TRUE(result.get_length().equals(ordinal_number::finite(2)));
    EXPECT_EQ(result.get(0), 2);
    EXPECT_EQ(result.get(1), 8);
}

TEST(Lab4LazySequenceFunctional, ReduceFinite) {
    int data[] = {1, 2, 3, 4};
    lazy_sequence<int> sequence(data, 4);
    EXPECT_EQ(sequence.reduce<int>(0, sum_int_values), 10);
    EXPECT_EQ(sequence.reduce<int>(1, product_int_values), 24);
}

TEST(Lab4LazySequenceFunctional, ReduceInfiniteLimited) {
    lazy_sequence<int> sequence = make_fibonacci();
    EXPECT_EQ(sequence.reduce<int>(0, sum_int_values, 6), 20);
}

TEST(Lab4LazySequenceFunctional, ZipFinite) {
    int first_data[] = {1, 2, 3};
    int second_data[] = {10, 20};
    lazy_sequence<int> first(first_data, 3);
    lazy_sequence<int> second(second_data, 2);
    lazy_sequence<std::pair<int, int>> zipped = first.zip(second);
    EXPECT_TRUE(zipped.get_length().equals(ordinal_number::finite(2)));
    EXPECT_EQ(zipped.get(0).first, 1);
    EXPECT_EQ(zipped.get(0).second, 10);
    EXPECT_EQ(zipped.get(1).first, 2);
    EXPECT_EQ(zipped.get(1).second, 20);
}

TEST(Lab4LazySequenceFunctional, ZipInfiniteLimited) {
    lazy_sequence<int> first = make_naturals();
    lazy_sequence<int> second = make_thousand_naturals();
    lazy_sequence<std::pair<int, int>> zipped = first.zip(second, 5);
    EXPECT_TRUE(zipped.get_length().equals(ordinal_number::finite(5)));
    EXPECT_EQ(zipped.get(4).first, 4);
    EXPECT_EQ(zipped.get(4).second, 1004);
}

TEST(Lab4LazySequenceInterleave, ThreeInfiniteSequences) {
    mutable_array_sequence<lazy_sequence<int>> sequences;
    sequences.append(make_naturals());
    sequences.append(make_thousand_naturals());
    sequences.append(make_negative_naturals());
    lazy_sequence<int> mixed = lazy_sequence<int>::interleave(sequences);
    EXPECT_TRUE(mixed.get_length().equals(ordinal_number::omega()));
    EXPECT_EQ(mixed.get(0), 0);
    EXPECT_EQ(mixed.get(1), 1000);
    EXPECT_EQ(mixed.get(2), 0);
    EXPECT_EQ(mixed.get(3), 1);
    EXPECT_EQ(mixed.get(4), 1001);
    EXPECT_EQ(mixed.get(5), -1);
    EXPECT_EQ(mixed.get(13), 1004);
}

TEST(Lab4LazySequenceInterleave, TwoFiniteSequences) {
    int a_data[] = {1, 2, 3};
    int b_data[] = {10, 20};
    lazy_sequence<int> a(a_data, 3);
    lazy_sequence<int> b(b_data, 2);
    mutable_array_sequence<lazy_sequence<int>> sequences;
    sequences.append(a);
    sequences.append(b);
    lazy_sequence<int> mixed = lazy_sequence<int>::interleave(sequences);
    EXPECT_TRUE(mixed.get_length().equals(ordinal_number::finite(4)));
    EXPECT_EQ(mixed.get(0), 1);
    EXPECT_EQ(mixed.get(1), 10);
    EXPECT_EQ(mixed.get(2), 2);
    EXPECT_EQ(mixed.get(3), 20);
    EXPECT_THROW(mixed.get(4), index_out_of_range_exception);
}

TEST(Lab4LazySequenceInterleave, EmptyCollectionThrows) {
    mutable_array_sequence<lazy_sequence<int>> sequences;
    EXPECT_THROW(lazy_sequence<int>::interleave(sequences), empty_collection_exception);
}

TEST(Lab4Generator, FiniteSequence) {
    int data[] = {7, 8};
    lazy_sequence<int> sequence(data, 2);
    generator<int> gen = sequence.get_generator();
    EXPECT_TRUE(gen.has_next());
    EXPECT_EQ(gen.get_next(), 7);
    EXPECT_EQ(gen.get_next(), 8);
    EXPECT_FALSE(gen.has_next());
    EXPECT_FALSE(gen.try_get_next().has_value());
    EXPECT_THROW(gen.get_next(), index_out_of_range_exception);
}

TEST(Lab4Generator, InfiniteSequence) {
    lazy_sequence<int> sequence = make_naturals();
    generator<int> gen = sequence.get_generator();
    EXPECT_TRUE(gen.has_next());
    EXPECT_EQ(gen.get_next(), 0);
    EXPECT_EQ(gen.get_next(), 1);
    EXPECT_EQ(gen.get_position(), static_cast<std::size_t>(2));
    EXPECT_TRUE(gen.try_get_next().has_value());
    EXPECT_TRUE(gen.has_next());
}

TEST(Lab4ReadOnlyStream, SequenceSource) {
    int data[] = {1, 2, 3};
    mutable_array_sequence<int> sequence(data, 3);
    read_only_stream<int> stream(sequence);
    EXPECT_EQ(stream.get_position(), static_cast<std::size_t>(0));
    EXPECT_TRUE(stream.is_can_seek());
    EXPECT_TRUE(stream.is_can_go_back());
    EXPECT_THROW(stream.read(), index_out_of_range_exception);
    stream.open();
    EXPECT_EQ(stream.read(), 1);
    EXPECT_EQ(stream.get_position(), static_cast<std::size_t>(1));
    EXPECT_EQ(stream.seek(2), static_cast<std::size_t>(2));
    EXPECT_EQ(stream.read(), 3);
    EXPECT_TRUE(stream.is_end_of_stream());
    EXPECT_THROW(stream.read(), index_out_of_range_exception);
    stream.close();
}

TEST(Lab4ReadOnlyStream, SeekBack) {
    int data[] = {1, 2, 3};
    mutable_array_sequence<int> sequence(data, 3);
    read_only_stream<int> stream(sequence);
    stream.open();
    EXPECT_EQ(stream.read(), 1);
    EXPECT_EQ(stream.read(), 2);
    EXPECT_EQ(stream.seek(0), static_cast<std::size_t>(0));
    EXPECT_EQ(stream.read(), 1);
    EXPECT_THROW(stream.seek(4), index_out_of_range_exception);
    stream.close();
}

TEST(Lab4ReadOnlyStream, LazySource) {
    lazy_sequence<int> sequence = make_fibonacci();
    read_only_stream<int> stream(sequence, 5);
    stream.open();
    EXPECT_EQ(stream.read(), 1);
    EXPECT_EQ(stream.seek(4), static_cast<std::size_t>(4));
    EXPECT_EQ(stream.read(), 5);
    stream.close();
}

TEST(Lab4ReadOnlyStream, FileSource) {
    std::filesystem::path path = std::filesystem::temp_directory_path() / "lab4_read_stream_test.txt";
    {
        std::ofstream out(path);
        out << "10\n20\n30\n";
    }
    read_only_stream<int> stream(path.string(), parse_int_line);
    stream.open();
    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.read(), 20);
    EXPECT_EQ(stream.seek(0), static_cast<std::size_t>(0));
    EXPECT_EQ(stream.read(), 10);
    stream.close();
    std::filesystem::remove(path);
}

TEST(Lab4WriteOnlyStream, MemoryStream) {
    write_only_stream<int> stream;
    EXPECT_THROW(stream.write(1), index_out_of_range_exception);
    stream.open();
    EXPECT_EQ(stream.write(10), static_cast<std::size_t>(1));
    EXPECT_EQ(stream.write(20), static_cast<std::size_t>(2));
    EXPECT_EQ(stream.get_position(), static_cast<std::size_t>(2));
    EXPECT_EQ(stream.data().size(), static_cast<std::size_t>(2));
    EXPECT_EQ(stream.data()[0], 10);
    EXPECT_EQ(stream.data()[1], 20);
    stream.close();
}

TEST(Lab4WriteOnlyStream, FileStream) {
    std::filesystem::path path = std::filesystem::temp_directory_path() / "lab4_write_stream_test.txt";
    write_only_stream<int> stream(path.string(), int_to_line);
    stream.open();
    stream.write(7);
    stream.write(8);
    stream.close();
    std::ifstream in(path);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_TRUE(content == "7\n8\n");
    in.close();
    std::filesystem::remove(path);
}

TEST(Lab4GrowingFileStream, ReadsExistingAndAppendedData) {
    std::filesystem::path path = std::filesystem::temp_directory_path() / "lab4_growing_file_stream_test.txt";
    {
        std::ofstream out(path);
        out << "1\n";
    }
    growing_file_read_stream<int> stream(path.string(), parse_int_line);
    stream.open();
    int value = 0;
    EXPECT_TRUE(stream.try_read(value));
    EXPECT_EQ(value, 1);
    EXPECT_FALSE(stream.try_read(value));
    {
        std::ofstream out(path, std::ios::app);
        out << "2\n3\n";
    }
    EXPECT_TRUE(stream.try_read(value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(stream.try_read(value));
    EXPECT_EQ(value, 3);
    EXPECT_EQ(stream.get_position(), static_cast<std::size_t>(3));
    stream.close();
    std::filesystem::remove(path);
}

TEST(Lab4GrowingFileStream, ReadThrowsWhenNoData) {
    std::filesystem::path path = std::filesystem::temp_directory_path() / "lab4_growing_file_stream_empty_test.txt";
    {
        std::ofstream out(path);
    }
    growing_file_read_stream<int> stream(path.string(), parse_int_line);
    EXPECT_THROW(stream.try_read(*static_cast<int*>(nullptr)), index_out_of_range_exception);
    stream.open();
    EXPECT_THROW(stream.read(), index_out_of_range_exception);
    stream.close();
    std::filesystem::remove(path);
}

TEST(Lab4OnlineStatistics, EmptySnapshot) {
    online_statistics_collector collector;
    online_statistics_snapshot snapshot = collector.snapshot();
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(0));
    EXPECT_TRUE(approx4(snapshot.mean, 0.0));
}

TEST(Lab4OnlineStatistics, SingleElement) {
    online_statistics_collector collector;
    collector.add(5);
    online_statistics_snapshot snapshot = collector.snapshot();
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(1));
    EXPECT_TRUE(approx4(snapshot.min, 5.0));
    EXPECT_TRUE(approx4(snapshot.max, 5.0));
    EXPECT_TRUE(approx4(snapshot.mean, 5.0));
    EXPECT_TRUE(approx4(snapshot.median, 5.0));
}

TEST(Lab4OnlineStatistics, OddCountMedian) {
    online_statistics_collector collector;
    collector.add(5);
    collector.add(1);
    collector.add(3);
    online_statistics_snapshot snapshot = collector.snapshot();
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(3));
    EXPECT_TRUE(approx4(snapshot.min, 1.0));
    EXPECT_TRUE(approx4(snapshot.max, 5.0));
    EXPECT_TRUE(approx4(snapshot.mean, 3.0));
    EXPECT_TRUE(approx4(snapshot.median, 3.0));
}

TEST(Lab4OnlineStatistics, EvenCountMedian) {
    online_statistics_collector collector;
    collector.add(10);
    collector.add(-2);
    collector.add(4);
    collector.add(8);
    online_statistics_snapshot snapshot = collector.snapshot();
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(4));
    EXPECT_TRUE(approx4(snapshot.min, -2.0));
    EXPECT_TRUE(approx4(snapshot.max, 10.0));
    EXPECT_TRUE(approx4(snapshot.mean, 5.0));
    EXPECT_TRUE(approx4(snapshot.median, 6.0));
}

TEST(Lab4OnlineStatistics, CollectFromStream) {
    double values[] = {5, 1, 3, 2, 4};
    mutable_array_sequence<double> sequence(values, 5);
    read_only_stream<double> stream(sequence);
    online_statistics_snapshot snapshot = collect_statistics(stream);
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(5));
    EXPECT_TRUE(approx4(snapshot.min, 1.0));
    EXPECT_TRUE(approx4(snapshot.max, 5.0));
    EXPECT_TRUE(approx4(snapshot.mean, 3.0));
    EXPECT_TRUE(approx4(snapshot.median, 3.0));
}

TEST(Lab4OnlineStatistics, FormatOutput) {
    double values[] = {1, 2, 3};
    mutable_array_sequence<double> sequence(values, 3);
    read_only_stream<double> stream(sequence);
    online_statistics_snapshot snapshot = collect_statistics(stream);
    std::ostringstream out;
    out << snapshot;
    EXPECT_TRUE(out.str().find("count=3") != std::string::npos);
    EXPECT_TRUE(out.str().find("median=2") != std::string::npos);
}

TEST(Lab4Format, OrdinalOutput) {
    std::ostringstream finite;
    finite << ordinal_number::finite(5);
    EXPECT_EQ(finite.str(), std::string("5"));
    std::ostringstream omega;
    omega << ordinal_number::omega(3, 7);
    EXPECT_EQ(omega.str(), std::string("omega*3+7"));
}

TEST(Lab4Stress, LargeNaturalSequence) {
    lazy_sequence<int> sequence = make_naturals();
    mutable_array_sequence<int> taken = sequence.take(10000);
    EXPECT_EQ(taken.get_length(), 10000);
    EXPECT_EQ(taken.get(0), 0);
    EXPECT_EQ(taken.get(9999), 9999);
    EXPECT_TRUE(sequence.get_materialized_count() <= 50);
}

TEST(Lab4Stress, LargeStatisticsStream) {
    mutable_array_sequence<double> sequence;
    for (int i = 1; i <= 1000; ++i) {
        sequence.append(static_cast<double>(i));
    }
    read_only_stream<double> stream(sequence);
    online_statistics_snapshot snapshot = collect_statistics(stream);
    EXPECT_EQ(snapshot.count, static_cast<std::size_t>(1000));
    EXPECT_TRUE(approx4(snapshot.min, 1.0));
    EXPECT_TRUE(approx4(snapshot.max, 1000.0));
    EXPECT_TRUE(approx4(snapshot.mean, 500.5));
    EXPECT_TRUE(approx4(snapshot.median, 500.5));
}

TEST(Lab4Stress, ChainedRules) {
    lazy_sequence<int> sequence = make_naturals().append(100).prepend(-1).insert_at(42, 3).map<int>(multiply_by_two_int);
    EXPECT_EQ(sequence.get(0), -2);
    EXPECT_EQ(sequence.get(1), 0);
    EXPECT_EQ(sequence.get(3), 84);
    EXPECT_EQ(sequence.get(4), 4);
}

void run_lab4_tests() {
    int argc = 0;
    char** argv = nullptr;
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::RUN_ALL_TESTS();
}
