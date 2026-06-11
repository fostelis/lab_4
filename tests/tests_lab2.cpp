#include <iostream>
#include <cassert>
#include <cmath>
#include "dynamic_array.h"
#include "linked_list.h"
#include "array_sequence.h"
#include "list_sequence.h"
#include "bit_sequence.h"
#include "algorithms.h"
#include "tests.h"
#include "io_utils.h"

using namespace std;

int tests_passed = 0;
int tests_failed = 0;

bool approx_equal(double a, double b, double eps = 1e-4) {
    return fabs(a - b) < eps;
}
void assert_equal(bool cond, const string& name) {
    if (cond) {
        cout << "[PASS] " << name << endl;
        ++tests_passed;
    }
    else {
        cout << "[FAIL] " << name << endl;
        ++tests_failed;
    }
}


void test_dynamic_array() {
    cout << "\n=== DynamicArray ===" << endl;
    int arr[] = {10, 20, 30};
    dynamic_array<int> da(arr, 3);
    assert_equal(da.get_size() == 3, "da: size after create");
    assert_equal(da.get(0) == 10, "da: get(0)");
    da.set(1, 25);
    assert_equal(da.get(1) == 25, "da: set(1,25)");
    da[2] = 35;
    assert_equal(da[2] == 35, "da: operator[] write");
    const dynamic_array<int>& cda = da;
    assert_equal(cda[0] == 10, "da: operator[] const");
    da.resize(5);
    assert_equal(da.get_size() == 5, "da: resize(5) size");
    assert_equal(da.get(0) == 10, "da: resize(5) keep old[0]");
    da.resize(2);
    assert_equal(da.get_size() == 2, "da: resize(2) size");
    da.resize(0);
    assert_equal(da.get_size() == 0, "da: resize(0) size");
    dynamic_array<int> empty;
    assert_equal(empty.get_size() == 0, "da: empty array size");
    int src_arr[] = {1, 2, 3};
    dynamic_array<int> src(src_arr, 3);
    dynamic_array<int> copy(src);
    assert_equal(copy.get_size() == 3, "da: copy ctor size");
    assert_equal(copy.get(0) == 1, "da: copy ctor get(0)");
    src.set(0, 99);
    assert_equal(copy.get(0) == 1, "da: deep copy independence");
    dynamic_array<int> assigned;
    assigned = src;
    assert_equal(assigned.get_size() == 3, "da: operator= size");
    assert_equal(assigned.get(0) == 99, "da: operator= get(0)");
    src.set(0, 88);
    assert_equal(assigned.get(0) == 99, "da: operator= deep copy");
    assigned = assigned;
    assert_equal(assigned.get_size() == 3, "da: self-assignment");
    dynamic_array<int> sized(4);
    assert_equal(sized.get_size() == 4, "da: sized ctor");
}

void test_dynamic_array_exceptions() {
    cout << "\n=== DynamicArray Exceptions ===" << endl;
    dynamic_array<int> da(3);
    bool caught = false;
    try {
        da.get(-1);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "da: get(-1) throws");
    caught = false;
    try {
        da.get(3);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "da: get(3) throws");
    caught = false;
    try {
        da.set(-1, 10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "da: set(-1) throws");
    caught = false;
    try {
        da.set(3, 10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "da: set(3) throws");
    caught = false;
    try {
        da[5] = 10;
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "da: operator[] out of range throws");
}


void test_linked_list() {
    cout << "\n=== LinkedList ===" << endl;
    linked_list<int> empty;
    assert_equal(empty.get_length() == 0, "ll: empty list length");
    linked_list<int> list;
    list.append(1);
    list.append(2);
    list.append(3);
    assert_equal(list.get_length() == 3, "ll: append length");
    assert_equal(list.get(0) == 1, "ll: get(0)");
    assert_equal(list.get_first() == 1, "ll: get_first");
    assert_equal(list.get_last() == 3, "ll: get_last");
    list.prepend(0);
    assert_equal(list.get_length() == 4, "ll: prepend length");
    assert_equal(list.get_first() == 0, "ll: prepend get_first");
    list.insert_at(99, 2);
    assert_equal(list.get_length() == 5, "ll: insert_at middle length");
    assert_equal(list.get(2) == 99, "ll: insert_at get(2)");
    list.insert_at(-1, 0);
    assert_equal(list.get_first() == -1, "ll: insert_at begin");
    list.insert_at(100, list.get_length());
    assert_equal(list.get_last() == 100, "ll: insert_at end");
    linked_list<int>* sub = list.get_sub_list(1, 3);
    assert_equal(sub->get_length() == 3, "ll: get_sub_list length");
    assert_equal(sub->get(0) == 0, "ll: get_sub_list[0]");
    delete sub;
    linked_list<int> other;
    other.append(200);
    other.append(300);
    linked_list<int>* concated = list.concat(&other);
    assert_equal(concated->get_length() == list.get_length() + 2, "ll: concat length");
    assert_equal(concated->get_last() == 300, "ll: concat last");
    delete concated;
    linked_list<int> copy(list);
    assert_equal(copy.get_length() == list.get_length(), "ll: copy ctor length");
    list.prepend(999);
    assert_equal(copy.get_first() != list.get_first(), "ll: deep copy independence");
    linked_list<int> assigned;
    assigned = list;
    assert_equal(assigned.get_length() == list.get_length(), "ll: operator= length");
    int arr[] = {10, 20, 30};
    linked_list<int> from_arr(arr, 3);
    assert_equal(from_arr.get_length() == 3, "ll: from array length");
    assert_equal(from_arr.get(0) == 10, "ll: from array get(0)");
}

void test_linked_list_exceptions() {
    cout << "\n=== LinkedList Exceptions ===" << endl;
    linked_list<int> empty;
    bool caught = false;
    try {
        empty.get_first();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: get_first on empty throws");
    caught = false;
    try {
        empty.get_last();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: get_last on empty throws");
    linked_list<int> list;
    list.append(1);
    caught = false;
    try {
        list.get(5);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: get(5) throws");
    caught = false;
    try {
        list.get(-1);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: get(-1) throws");
    caught = false;
    try {
        list.insert_at(10, 10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: insert_at(10) throws");
    caught = false;
    try {
        list.get_sub_list(0, 5);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ll: get_sub_list out of range throws");
}


void test_array_sequence() {
    cout << "\n=== ArraySequence ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    seq->append(1);
    seq->append(2);
    seq->append(3);
    assert_equal(seq->get_length() == 3, "as: append length");
    assert_equal(seq->get_first() == 1, "as: get_first");
    assert_equal(seq->get_last() == 3, "as: get_last");
    assert_equal(seq->get(0) == 1, "as: get(0)");
    seq->prepend(0);
    assert_equal(seq->get_length() == 4, "as: prepend length");
    assert_equal(seq->get_first() == 0, "as: prepend get_first");
    seq->insert_at(99, 2);
    assert_equal(seq->get_length() == 5, "as: insert_at length");
    assert_equal(seq->get(2) == 99, "as: insert_at get(2)");
    array_sequence<int>* sub = seq->get_subsequence(1, 3);
    assert_equal(sub->get_length() == 3, "as: get_subsequence length");
    assert_equal(sub->get(0) == 1, "as: get_subsequence[0]");
    delete sub;
    array_sequence<int>* cloned = seq->clone();
    assert_equal(cloned->get_length() == seq->get_length(), "as: clone length");
    int len_before = seq->get_length();
    mutable_array_sequence<int>* other = new mutable_array_sequence<int>();
    other->append(100);
    other->append(200);
    array_sequence<int>* concated = seq->concat(*other);
    assert_equal(concated == seq, "as: concat returns same object for mutable");
    assert_equal(concated->get_length() == len_before + 2, "as: concat length");
    assert_equal(cloned->get_length() == len_before, "as: clone unchanged after concat");
    delete other;
    delete cloned;
    delete seq;
    int arr[] = {5, 6, 7};
    mutable_array_sequence<int>* from_arr = new mutable_array_sequence<int>(arr, 3);
    assert_equal(from_arr->get_length() == 3, "as: from array length");
    assert_equal(from_arr->get(0) == 5, "as: from array get(0)");
    delete from_arr;
}

void test_array_sequence_exceptions() {
    cout << "\n=== ArraySequence Exceptions ===" << endl;
    mutable_array_sequence<int>* empty = new mutable_array_sequence<int>();
    bool caught = false;
    try {
        empty->get_first();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "as: get_first on empty throws");
    caught = false;
    try {
        empty->get_last();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "as: get_last on empty throws");
    empty->append(1);
    caught = false;
    try {
        empty->get(5);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "as: get(5) throws");
    caught = false;
    try {
        empty->insert_at(10, 10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "as: insert_at(10) throws");
    caught = false;
    try {
        empty->get_subsequence(0, 5);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "as: get_subsequence out of range throws");
    delete empty;
}


void test_list_sequence() {
    cout << "\n=== ListSequence ===" << endl;
    mutable_list_sequence<int>* seq = new mutable_list_sequence<int>();
    seq->append(1);
    seq->append(2);
    seq->append(3);
    assert_equal(seq->get_length() == 3, "ls: append length");
    assert_equal(seq->get_first() == 1, "ls: get_first");
    assert_equal(seq->get_last() == 3, "ls: get_last");
    seq->prepend(0);
    assert_equal(seq->get_length() == 4, "ls: prepend length");
    assert_equal(seq->get_first() == 0, "ls: prepend get_first");
    seq->insert_at(99, 2);
    assert_equal(seq->get_length() == 5, "ls: insert_at length");
    assert_equal(seq->get(2) == 99, "ls: insert_at get(2)");
    list_sequence<int>* sub = seq->get_subsequence(1, 3);
    assert_equal(sub->get_length() == 3, "ls: get_subsequence length");
    delete sub;
    list_sequence<int>* cloned = seq->clone();
    assert_equal(cloned->get_length() == seq->get_length(), "ls: clone length");
    int len_before = seq->get_length();
    mutable_list_sequence<int>* other = new mutable_list_sequence<int>();
    other->append(100);
    other->append(200);
    list_sequence<int>* concated = seq->concat(*other);
    assert_equal(concated == seq, "ls: concat returns same object for mutable");
    assert_equal(concated->get_length() == len_before + 2, "ls: concat length");
    assert_equal(cloned->get_length() == len_before, "ls: clone unchanged after concat");
    delete other;
    delete cloned;
    delete seq;
    int arr[] = {5, 6, 7};
    mutable_list_sequence<int>* from_arr = new mutable_list_sequence<int>(arr, 3);
    assert_equal(from_arr->get_length() == 3, "ls: from array length");
    delete from_arr;
}

void test_list_sequence_exceptions() {
    cout << "\n=== ListSequence Exceptions ===" << endl;
    mutable_list_sequence<int>* empty = new mutable_list_sequence<int>();
    bool caught = false;
    try {
        empty->get_first();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "ls: get_first on empty throws");
    caught = false;
    try {
        empty->get_last();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "ls: get_last on empty throws");
    empty->append(1);
    caught = false;
    try {
        empty->get(5);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ls: get(5) throws");
    caught = false;
    try {
        empty->insert_at(10, 10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "ls: insert_at(10) throws");
    delete empty;
}


void test_immutable_list_sequence() {
    cout << "\n=== Immutable ListSequence ===" << endl;
    immutable_list_sequence<int>* imm = new immutable_list_sequence<int>();
    immutable_list_sequence<int>* imm1 = dynamic_cast<immutable_list_sequence<int>*>(imm->append(1));
    immutable_list_sequence<int>* imm2 = dynamic_cast<immutable_list_sequence<int>*>(imm1->append(2));
    immutable_list_sequence<int>* imm3 = dynamic_cast<immutable_list_sequence<int>*>(imm2->append(3));
    assert_equal(imm->get_length() == 0, "imm_ls: original empty");
    assert_equal(imm1->get_length() == 1, "imm_ls: after 1 append length");
    assert_equal(imm1->get(0) == 1, "imm_ls: after 1 append get(0)");
    assert_equal(imm2->get_length() == 2, "imm_ls: after 2 append length");
    assert_equal(imm3->get_length() == 3, "imm_ls: after 3 append length");
    assert_equal(imm2->get_length() == 2, "imm_ls: imm2 unchanged");
    immutable_list_sequence<int>* imm_pre = dynamic_cast<immutable_list_sequence<int>*>(imm2->prepend(0));
    assert_equal(imm_pre->get_length() == 3, "imm_ls: prepend length");
    assert_equal(imm_pre->get_first() == 0, "imm_ls: prepend first");
    assert_equal(imm2->get_length() == 2, "imm_ls: prepend original unchanged");
    delete imm_pre;
    immutable_list_sequence<int>* imm_ins = dynamic_cast<immutable_list_sequence<int>*>(imm2->insert_at(99, 1));
    assert_equal(imm_ins->get_length() == 3, "imm_ls: insert_at length");
    assert_equal(imm_ins->get(1) == 99, "imm_ls: insert_at get(1)");
    delete imm_ins;
    immutable_list_sequence<int>* other = new immutable_list_sequence<int>();
    immutable_list_sequence<int>* other1 = dynamic_cast<immutable_list_sequence<int>*>(other->append(100));
    immutable_list_sequence<int>* concated = dynamic_cast<immutable_list_sequence<int>*>(imm2->concat(*other1));
    assert_equal(concated->get_length() == 3, "imm_ls: concat length");
    assert_equal(concated->get(2) == 100, "imm_ls: concat get(2)");
    delete imm;
    delete imm1;
    delete imm2;
    delete imm3;
    delete other;
    delete other1;
    delete concated;
}


void test_mutable_vs_immutable() {
    cout << "\n=== Mutable vs Immutable ===" << endl;
    mutable_array_sequence<int>* mut = new mutable_array_sequence<int>();
    mut->append(1);
    int orig_len = mut->get_length();
    mut->append(2);
    assert_equal(mut->get_length() == orig_len + 1, "mut_vs_imm: mutable modifies self");
    assert_equal(mut->get(1) == 2, "mut_vs_imm: mutable new element");
    delete mut;
    immutable_array_sequence<int>* imm_empty = new immutable_array_sequence<int>();
    immutable_array_sequence<int>* imm1 = dynamic_cast<immutable_array_sequence<int>*>(imm_empty->append(1));
    immutable_array_sequence<int>* imm2 = dynamic_cast<immutable_array_sequence<int>*>(imm1->append(2));
    int orig_imm = imm2->get_length();
    immutable_array_sequence<int>* imm3 = dynamic_cast<immutable_array_sequence<int>*>(imm2->append(3));
    assert_equal(imm2->get_length() == orig_imm, "mut_vs_imm: immutable original unchanged");
    assert_equal(imm3->get_length() == orig_imm + 1, "mut_vs_imm: immutable new object longer");
    assert_equal(imm3->get(2) == 3, "mut_vs_imm: immutable new element");
    assert_equal(imm1->get_length() == 1, "mut_vs_imm: imm1 unchanged");
    assert_equal(imm2->get_length() == 2, "mut_vs_imm: imm2 unchanged");
    delete imm_empty;
    delete imm1;
    delete imm2;
    delete imm3;
}


void test_bit_sequence() {
    cout << "\n=== BitSequence ===" << endl;
    bit_sequence* bs = new bit_sequence();
    bs->append(bit(true));
    bs->append(bit(false));
    bs->append(bit(true));
    assert_equal(bs->get_length() == 3, "bs: append length");
    assert_equal(bs->get(0).get_value() == true, "bs: get(0)");
    assert_equal(bs->get(1).get_value() == false, "bs: get(1)");
    assert_equal(bs->get_first().get_value() == true, "bs: get_first");
    assert_equal(bs->get_last().get_value() == true, "bs: get_last");
    bs->prepend(bit(false));
    assert_equal(bs->get_length() == 4, "bs: prepend length");
    assert_equal(bs->get_first().get_value() == false, "bs: prepend get_first");
    bs->insert_at(bit(true), 2);
    assert_equal(bs->get_length() == 5, "bs: insert_at length");
    assert_equal(bs->get(2).get_value() == true, "bs: insert_at get(2)");
    bit_sequence* sub = bs->get_subsequence(1, 3);
    assert_equal(sub->get_length() == 3, "bs: get_subsequence length");
    delete sub;
    delete bs;

    bit_sequence* bs1 = new bit_sequence();
    bit_sequence* bs2 = new bit_sequence();
    bs1->append(bit(true));
    bs1->append(bit(false));
    bs1->append(bit(true));
    bs1->append(bit(false));
    bs2->append(bit(true));
    bs2->append(bit(true));
    bs2->append(bit(false));
    bs2->append(bit(false));
    bit_sequence* and_res = bs1->bit_and(bs2);
    assert_equal(and_res->get_length() == 4, "bs: AND length");
    assert_equal(and_res->get(0).get_value() == true, "bs: AND[0] 1&1=1");
    assert_equal(and_res->get(1).get_value() == false, "bs: AND[1] 0&1=0");
    delete and_res;
    bit_sequence* or_res = bs1->bit_or(bs2);
    assert_equal(or_res->get(0).get_value() == true, "bs: OR[0] 1|1=1");
    assert_equal(or_res->get(3).get_value() == false, "bs: OR[3] 0|0=0");
    delete or_res;
    bit_sequence* xor_res = bs1->bit_xor(bs2);
    assert_equal(xor_res->get(1).get_value() == true, "bs: XOR[1] 0^1=1");
    assert_equal(xor_res->get(3).get_value() == false, "bs: XOR[3] 0^0=0");
    delete xor_res;
    bit_sequence* not_res = bs1->bit_not();
    assert_equal(not_res->get(0).get_value() == false, "bs: NOT[0] !1=0");
    assert_equal(not_res->get(1).get_value() == true, "bs: NOT[1] !0=1");
    delete not_res;
    sequence<bit>* concated = bs1->concat(*bs2);
    assert_equal(concated->get_length() == 8, "bs: concat length");
    delete concated;
    bit_sequence* cloned = dynamic_cast<bit_sequence*>(bs1->clone());
    assert_equal(cloned->get_length() == bs1->get_length(), "bs: clone length");
    delete cloned;
    delete bs1;
    delete bs2;
}

void test_bit_sequence_exceptions() {
    cout << "\n=== BitSequence Exceptions ===" << endl;
    bit_sequence* empty = new bit_sequence();
    bool caught = false;
    try {
        empty->get_first();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "bs: get_first on empty throws");
    caught = false;
    try {
        empty->get_last();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "bs: get_last on empty throws");
    delete empty;
    bit_sequence* bs1 = new bit_sequence();
    bit_sequence* bs2 = new bit_sequence();
    bs1->append(bit(true));
    bs1->append(bit(false));
    bs2->append(bit(true));
    caught = false;
    try {
        bs1->bit_and(bs2);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "bs: bit_and size mismatch throws");
    caught = false;
    try {
        bs1->bit_or(bs2);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "bs: bit_or size mismatch throws");
    caught = false;
    try {
        bs1->bit_xor(bs2);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "bs: bit_xor size mismatch throws");
    delete bs1;
    delete bs2;
}


void test_operators() {
    cout << "\n=== Operators ===" << endl;
    mutable_array_sequence<int>* s1 = new mutable_array_sequence<int>();
    mutable_array_sequence<int>* s2 = new mutable_array_sequence<int>();
    s1->append(1);
    s1->append(2);
    s2->append(1);
    s2->append(2);
    assert_equal(*s1 == *s2, "op: operator== equal");
    s2->append(3);
    assert_equal(!(*s1 == *s2), "op: operator== not equal");
    assert_equal(*s1 != *s2, "op: operator!=");
    mutable_array_sequence<int>* s3 = new mutable_array_sequence<int>();
    s3->append(3);
    s3->append(4);
    sequence<int>* concat = *s1 + *s3;
    assert_equal(concat->get_length() == 4, "op: operator+ length");
    assert_equal(concat->get(0) == 1, "op: operator+ get(0)");
    assert_equal(concat->get(3) == 4, "op: operator+ get(3)");
    delete concat;
    immutable_array_sequence<int>* imm = new immutable_array_sequence<int>();
    immutable_array_sequence<int>* imm1 = dynamic_cast<immutable_array_sequence<int>*>(imm->append(5));
    sequence<int>* concat_imm = *imm1 + *s3;
    assert_equal(concat_imm->get_length() == 3, "op: operator+ with immutable length");
    assert_equal(concat_imm->get(0) == 5, "op: operator+ with immutable get(0)");
    delete concat_imm;
    delete imm;
    delete imm1;
    delete s1;
    delete s2;
    delete s3;
}


void test_map_reduce_core() {
    cout << "\n=== Map-Reduce Core ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    static auto doubler = [](const int& x) -> int { return x * 2; };
    sequence<int>* doubled = seq->map<int>(static_cast<int(*)(const int&)>(doubler));
    assert_equal(doubled->get_length() == 5, "mr: map length");
    assert_equal(doubled->get(0) == 2, "mr: map[0]");
    assert_equal(doubled->get(4) == 10, "mr: map[4]");
    delete doubled;
    sequence<int>* tripled = seq->map<int>(std::function<int(const int&)>([](const int& x) { return x * 3; }));
    assert_equal(tripled->get(0) == 3, "mr: map with function[0]");
    assert_equal(tripled->get(4) == 15, "mr: map with function[4]");
    delete tripled;
    sequence<int>* filtered = seq->where(std::function<bool(const int&)>([](const int& x) { return x > 3; }));
    assert_equal(filtered->get_length() == 2, "mr: where length");
    assert_equal(filtered->get(0) == 4, "mr: where[0]");
    delete filtered;
    static auto is_even = [](const int& x) -> bool { return x % 2 == 0; };
    sequence<int>* evens = seq->where(static_cast<bool(*)(const int&)>(is_even));
    assert_equal(evens->get_length() == 2, "mr: where even length");
    assert_equal(evens->get(0) == 2, "mr: where even[0]");
    delete evens;
    int sum = seq->reduce<int>(std::function<int(const int&, const int&)>([](const int& a, const int& x) { return a + x; }), 0);
    assert_equal(sum == 15, "mr: reduce sum");
    int prod = seq->reduce<int>(std::function<int(const int&, const int&)>([](const int& a, const int& x) { return a * x; }), 1);
    assert_equal(prod == 120, "mr: reduce product");
    static auto adder = [](const int& a, const int& x) -> int { return a + x; };
    int sum2 = seq->reduce<int>(static_cast<int(*)(const int&, const int&)>(adder), 0);
    assert_equal(sum2 == 15, "mr: reduce with func ptr");
    delete seq;
}


void test_try_semantics() {
    cout << "\n=== Try-Semantics and Option ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    option<int> first = seq->try_get_first();
    assert_equal(first.is_some(), "try: get_first some");
    assert_equal(first.value() == 1, "try: get_first value");
    option<int> found = seq->try_get_first([](const int& x) { return x > 3; });
    assert_equal(found.is_some(), "try: find >3 some");
    assert_equal(found.value() == 4, "try: find >3 value");
    option<int> not_found = seq->try_get_first([](const int& x) { return x > 10; });
    assert_equal(not_found.is_none(), "try: find >10 none");
    option<int> none = option<int>::none();
    assert_equal(none.is_none(), "try: option::none");
    option<int> some = option<int>::some(42);
    assert_equal(some.is_some(), "try: option::some");
    assert_equal(some.value() == 42, "try: option::some value");
    delete seq;
}


void test_zip() {
    cout << "\n=== Zip ===" << endl;
    mutable_array_sequence<int>* s1 = new mutable_array_sequence<int>();
    s1->append(1);
    s1->append(2);
    s1->append(3);
    mutable_array_sequence<char>* s2 = new mutable_array_sequence<char>();
    s2->append('a');
    s2->append('b');
    s2->append('c');
    s2->append('d');
    sequence<pair<int, char>>* zipped = s1->zip(*s2);
    assert_equal(zipped->get_length() == 3, "zip: length min(3,4)=3");
    assert_equal(zipped->get(0).first == 1 && zipped->get(0).second == 'a', "zip: pair[0]");
    assert_equal(zipped->get(2).first == 3 && zipped->get(2).second == 'c', "zip: pair[2]");
    delete zipped;
    mutable_array_sequence<char>* s3 = new mutable_array_sequence<char>();
    s3->append('x');
    sequence<pair<int, char>>* zipped2 = s1->zip(*s3);
    assert_equal(zipped2->get_length() == 1, "zip: length min(3,1)=1");
    assert_equal(zipped2->get(0).first == 1 && zipped2->get(0).second == 'x', "zip: pair[0] short");
    delete zipped2;
    delete s3;
    delete s1;
    delete s2;
}


void test_extended_map_reduce() {
    cout << "\n=== Extended Map-Reduce ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    sequence<int>* skipped = seq->skip(2);
    assert_equal(skipped->get_length() == 3, "ext: skip(2) length");
    assert_equal(skipped->get(0) == 3, "ext: skip(2)[0]");
    assert_equal(skipped->get(2) == 5, "ext: skip(2)[2]");
    delete skipped;
    sequence<int>* skipped_all = seq->skip(10);
    assert_equal(skipped_all->get_length() == 0, "ext: skip(10) empty");
    delete skipped_all;
    sequence<int>* skipped_neg = seq->skip(-2);
    assert_equal(skipped_neg->get_length() == 5, "ext: skip(-2) treats as 0");
    delete skipped_neg;
    sequence<int>* taken = seq->take(3);
    assert_equal(taken->get_length() == 3, "ext: take(3) length");
    assert_equal(taken->get(0) == 1, "ext: take(3)[0]");
    assert_equal(taken->get(2) == 3, "ext: take(3)[2]");
    delete taken;
    sequence<int>* taken_all = seq->take(10);
    assert_equal(taken_all->get_length() == 5, "ext: take(10) caps at length");
    delete taken_all;
    sequence<int>* taken_neg = seq->take(-2);
    assert_equal(taken_neg->get_length() == 0, "ext: take(-2) empty");
    delete taken_neg;
    sequence<int>* mapped = seq->map_with_index<int>(std::function<int(const int&, int)>([](const int& v, int i) { return v + i; }));
    assert_equal(mapped->get_length() == 5, "ext: map_with_index length");
    assert_equal(mapped->get(0) == 1, "ext: map_with_index[0] 1+0=1");
    assert_equal(mapped->get(4) == 9, "ext: map_with_index[4] 5+4=9");
    delete mapped;
    mutable_array_sequence<int>* to_split = new mutable_array_sequence<int>();
    to_split->append(1);
    to_split->append(2);
    to_split->append(0);
    to_split->append(3);
    to_split->append(0);
    to_split->append(4);
    to_split->append(5);
    sequence<sequence<int>*>* splitted = to_split->split([](const int& x) { return x == 0; });
    assert_equal(splitted->get_length() == 3, "ext: split length (3 fragments)");
    assert_equal(splitted->get(0)->get_length() == 2, "ext: split fragment0 length");
    assert_equal(splitted->get(0)->get(0) == 1, "ext: split fragment0[0]");
    assert_equal(splitted->get(1)->get(0) == 3, "ext: split fragment1[0]");
    assert_equal(splitted->get(2)->get(1) == 5, "ext: split fragment2[1]");
    delete_split_result(splitted);
    delete to_split;
    mutable_array_sequence<int>* slice_seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        slice_seq->append(i);
    }
    sequence<int>* sliced = slice_seq->slice(1, 2, nullptr);
    assert_equal(sliced->get_length() == 3, "ext: slice(1,2,null) length");
    assert_equal(sliced->get(0) == 1, "ext: slice(1,2,null)[0]");
    assert_equal(sliced->get(1) == 4, "ext: slice(1,2,null)[1]");
    delete sliced;
    mutable_array_sequence<int>* repl = new mutable_array_sequence<int>();
    repl->append(9);
    repl->append(10);
    sequence<int>* sliced_repl = slice_seq->slice(1, 2, repl);
    assert_equal(sliced_repl->get_length() == 5, "ext: slice(1,2,repl) length");
    assert_equal(sliced_repl->get(1) == 9, "ext: slice with repl[1]");
    assert_equal(sliced_repl->get(2) == 10, "ext: slice with repl[2]");
    delete sliced_repl;
    delete repl;
    sequence<int>* sliced_neg = slice_seq->slice(-2, 1, nullptr);
    assert_equal(sliced_neg->get_length() == 4, "ext: slice(-2,1,null) length");
    assert_equal(sliced_neg->get(3) == 5, "ext: slice(-2)[3]");
    delete sliced_neg;
    delete slice_seq;
    delete seq;
}


void test_algorithms() {
    cout << "\n=== Algorithms ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    stats<int> st = get_stats(*seq);
    assert_equal(st.min_val == 1, "alg: stats min");
    assert_equal(st.max_val == 5, "alg: stats max");
    assert_equal(approx_equal(st.avg_val, 3.0), "alg: stats avg");
    mutable_array_sequence<int>* empty = new mutable_array_sequence<int>();
    stats<int> st_empty = get_stats(*empty);
    assert_equal(st_empty.min_val == 0, "alg: empty stats min default");
    assert_equal(st_empty.avg_val == 0, "alg: empty stats avg default");
    delete empty;
    sequence<double>* avg = moving_average_3(*seq);
    assert_equal(avg->get_length() == 5, "alg: moving_avg length");
    assert_equal(approx_equal(avg->get(0), 1.5), "alg: moving_avg[0] (1+2)/2=1.5");
    assert_equal(approx_equal(avg->get(1), 2.0), "alg: moving_avg[1] (1+2+3)/3=2.0");
    assert_equal(approx_equal(avg->get(4), 4.5), "alg: moving_avg[4] (4+5)/2=4.5");
    delete avg;
    sequence<int>* refl = sum_with_reflection(*seq);
    assert_equal(refl->get_length() == 5, "alg: reflection length");
    assert_equal(refl->get(0) == 6, "alg: reflection[0] 1+5=6");
    assert_equal(refl->get(2) == 6, "alg: reflection[2] 3+3=6");
    delete refl;
    mutable_array_sequence<int>* inv_seq = new mutable_array_sequence<int>();
    inv_seq->append(2);
    inv_seq->append(1);
    inv_seq->append(3);
    inv_seq->append(1);
    int inversions = count_inversions(*inv_seq);
    assert_equal(inversions == 3, "alg: count_inversions [2,1,3,1] -> 3");
    delete inv_seq;
    sequence<sequence<int>*>* prefixes = get_all_prefixes(*seq);
    assert_equal(prefixes->get_length() == 5, "alg: prefixes count");
    assert_equal(prefixes->get(0)->get_length() == 1, "alg: prefix[0] length");
    assert_equal(prefixes->get(0)->get(0) == 1, "alg: prefix[0][0]");
    assert_equal(prefixes->get(2)->get_length() == 3, "alg: prefix[2] length");
    assert_equal(prefixes->get(2)->get(2) == 3, "alg: prefix[2][2]");
    for (int i = 0; i < prefixes->get_length(); ++i) {
        delete prefixes->get(i);
    }
    delete prefixes;
    delete seq;
}


void test_exceptions() {
    cout << "\n=== Exceptions ===" << endl;
    dynamic_array<int> da(3);
    bool caught = false;
    try {
        da.get(10);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "exc: da.get(10) throws");
    linked_list<int> empty_list;
    caught = false;
    try {
        empty_list.get_first();
    }
    catch (const empty_collection_exception&) {
        caught = true;
    }
    assert_equal(caught, "exc: empty_list.get_first() throws empty_collection");
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    seq->append(1);
    caught = false;
    try {
        seq->slice(5, 1, nullptr);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "exc: slice(5) throws");
    caught = false;
    try {
        seq->slice(-10, 1, nullptr);
    }
    catch (const index_out_of_range_exception&) {
        caught = true;
    }
    assert_equal(caught, "exc: slice(-10) throws");
    delete seq;
}


void test_crtp_implementation() {
    cout << "\n=== CRTP Implementation ===" << endl;
    mutable_array_sequence_crtp<int>* mut = new mutable_array_sequence_crtp<int>();
    mut->append(1);
    mut->append(2);
    assert_equal(mut->get_length() == 2, "crtp: mutable length");
    assert_equal(mut->get(0) == 1, "crtp: mutable get(0)");
    mut->prepend(0);
    assert_equal(mut->get_length() == 3, "crtp: mutable prepend length");
    assert_equal(mut->get_first() == 0, "crtp: mutable prepend first");
    mut->insert_at(99, 2);
    assert_equal(mut->get(2) == 99, "crtp: mutable insert_at");
    sequence<int>* cloned = mut->clone();
    assert_equal(cloned->get_length() == mut->get_length(), "crtp: mutable clone length");
    delete cloned;
    delete mut;
    immutable_array_sequence_crtp<int>* imm_empty = new immutable_array_sequence_crtp<int>();
    immutable_array_sequence_crtp<int>* imm1 = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm_empty->append(1));
    immutable_array_sequence_crtp<int>* imm2 = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm1->append(2));
    int orig_len = imm2->get_length();
    immutable_array_sequence_crtp<int>* imm3 = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm2->append(3));
    assert_equal(imm2->get_length() == orig_len, "crtp: immutable unchanged");
    assert_equal(imm3->get_length() == orig_len + 1, "crtp: immutable new longer");
    immutable_array_sequence_crtp<int>* imm_pre = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm2->prepend(0));
    assert_equal(imm_pre->get_length() == 3, "crtp: immutable prepend length");
    assert_equal(imm_pre->get_first() == 0, "crtp: immutable prepend first");
    assert_equal(imm2->get_length() == 2, "crtp: immutable prepend original unchanged");
    delete imm_pre;
    immutable_array_sequence_crtp<int>* imm_ins = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm2->insert_at(99, 1));
    assert_equal(imm_ins->get_length() == 3, "crtp: immutable insert_at length");
    assert_equal(imm_ins->get(1) == 99, "crtp: immutable insert_at get(1)");
    delete imm_ins;
    immutable_array_sequence_crtp<int>* other = new immutable_array_sequence_crtp<int>();
    immutable_array_sequence_crtp<int>* other1 = dynamic_cast<immutable_array_sequence_crtp<int>*>(other->append(100));
    immutable_array_sequence_crtp<int>* concated = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm2->concat(*other1));
    assert_equal(concated->get_length() == 3, "crtp: immutable concat length");
    assert_equal(concated->get(2) == 100, "crtp: immutable concat last");
    delete imm_empty;
    delete imm1;
    delete imm2;
    delete imm3;
    delete other;
    delete other1;
    delete concated;
    cout << "[INFO] CRTP pattern demonstrated for bonus points" << endl;
}


void run_all_tests() {
    tests_passed = tests_failed = 0;
    cout << "\n========== RUNNING TESTS ==========" << endl;
    test_dynamic_array();
    test_dynamic_array_exceptions();
    test_linked_list();
    test_linked_list_exceptions();
    test_array_sequence();
    test_array_sequence_exceptions();
    test_list_sequence();
    test_list_sequence_exceptions();
    test_immutable_list_sequence();
    test_mutable_vs_immutable();
    test_bit_sequence();
    test_bit_sequence_exceptions();
    test_operators();
    test_map_reduce_core();
    test_try_semantics();
    test_zip();
    test_extended_map_reduce();
    test_algorithms();
    test_exceptions();
    test_crtp_implementation();
    cout << "\n========== RESULTS ==========" << endl;
    cout << "Passed: " << tests_passed << endl;
    cout << "Failed: " << tests_failed << endl;
    cout << "Total:  " << (tests_passed + tests_failed) << endl;
    cout << "=============================" << endl;
}
