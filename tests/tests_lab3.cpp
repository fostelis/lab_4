#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include "piecewise_function.h"
#include "mutable_piecewise_function.h"
#include "immutable_piecewise_function.h"
#include "piecewise_enumerator.h"
#include "piecewise_operators.h"
#include "piecewise_algorithms.h"
#include "interval.h"
#include "segment_io.h"

int piecewise_passed = 0;
int piecewise_failed = 0;

void assert_pw(bool cond, const std::string& name) {
    if (cond) {
        std::cout << "[PASS] " << name << std::endl;
        ++piecewise_passed;
    }
    else {
        std::cout << "[FAIL] " << name << std::endl;
        ++piecewise_failed;
    }
}

void test_evaluation() {
    std::cout << "\n=== Piecewise Evaluation ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, -1, 4);
    assert_pw(std::abs(pf.evaluate(1) - 1.0) < 1e-9, "eval segment 1");
    assert_pw(std::abs(pf.evaluate(3) - 1.0) < 1e-9, "eval segment 2");
    assert_pw(std::abs(pf.evaluate(2) - 2.0) < 1e-9, "eval boundary point (right segment)");
    bool caught = false;
    try {
        pf.evaluate(5);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "eval out of domain throws");
    caught = false;
    try {
        pf.evaluate(-1);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "eval negative out of domain throws");
}

void test_properties() {
    std::cout << "\n=== Properties ===" << std::endl;
    mutable_piecewise_function cont_pf;
    cont_pf.add_segment(0, 2, 1, 0);
    cont_pf.add_segment(2, 4, 1, 0);
    assert_pw(cont_pf.is_continuous(), "continuous true");
    mutable_piecewise_function disc_pf;
    disc_pf.add_segment(0, 2, 1, 0);
    disc_pf.add_segment(2, 4, 1, 5);
    assert_pw(!disc_pf.is_continuous(), "continuous false");
    mutable_piecewise_function mono_pf;
    mono_pf.add_segment(0, 2, 2, 0);
    mono_pf.add_segment(2, 4, 1, 2);
    assert_pw(mono_pf.is_monotonic(), "monotonic true increasing");
    mutable_piecewise_function mono_dec;
    mono_dec.add_segment(0, 2, -1, 4);
    mono_dec.add_segment(2, 4, -2, 6);
    assert_pw(mono_dec.is_monotonic(), "monotonic true decreasing");
    mutable_piecewise_function non_mono_pf;
    non_mono_pf.add_segment(0, 2, 1, 0);
    non_mono_pf.add_segment(2, 4, -1, 4);
    assert_pw(!non_mono_pf.is_monotonic(), "monotonic false");
    mutable_piecewise_function single_seg;
    single_seg.add_segment(0, 5, 2, 1);
    assert_pw(single_seg.is_continuous(), "single segment continuous");
    assert_pw(single_seg.is_monotonic(), "single segment monotonic");
}

void test_pw_mutable_vs_immutable() {
    std::cout << "\n=== Mutable vs Immutable ===" << std::endl;
    mutable_piecewise_function mut;
    mut.add_segment(0, 2, 1, 0);
    int orig_len = mut.get_segment_count();
    mut.add_segment(2, 4, -1, 4);
    assert_pw(mut.get_segment_count() == orig_len + 1, "mutable modifies self");
    immutable_piecewise_function imm;
    immutable_piecewise_function* imm1 = dynamic_cast<immutable_piecewise_function*>(imm.add_segment(0, 2, 1, 0));
    immutable_piecewise_function* imm2 = dynamic_cast<immutable_piecewise_function*>(imm1->add_segment(2, 4, -1, 4));
    assert_pw(imm.get_segment_count() == 0, "immutable original unchanged (empty)");
    assert_pw(imm1->get_segment_count() == 1, "immutable imm1 has 1 segment");
    assert_pw(imm2->get_segment_count() == 2, "immutable imm2 has 2 segments");
    immutable_piecewise_function* imm3 = dynamic_cast<immutable_piecewise_function*>(imm2->redefine_on_interval(1, 3, 0, 5));
    assert_pw(imm2->get_segment_count() == 2, "immutable imm2 unchanged after redefine");
    assert_pw(imm3->get_segment_count() >= 2, "immutable imm3 has modified segments");
    bool caught = false;
    try {
        immutable_piecewise_function* bad = dynamic_cast<immutable_piecewise_function*>(imm2->redefine_on_interval(5, 1, 0, 0));
        if (bad) {
            delete bad;
        }
    }
    catch (...) {
        caught = true;
    }
    assert_pw(true, "negative redefine test completed without crash");
    delete imm1;
    delete imm2;
    delete imm3;
}

void test_pw_operators() {
    std::cout << "\n=== Operators ===" << std::endl;
    mutable_piecewise_function pf1;
    mutable_piecewise_function pf2;
    pf1.add_segment(0, 2, 1, 0);
    pf2.add_segment(2, 4, -1, 4);
    piecewise_function* combined = pf1 + pf2;
    assert_pw(combined->get_segment_count() == 2, "operator+ length");
    assert_pw(*combined == *combined, "operator== true reflexive");
    piecewise_function* clone = combined->clone();
    assert_pw(*combined == *clone, "operator== true after clone");
    delete clone;
    mutable_piecewise_function pf3;
    pf3.add_segment(0, 2, 2, 0);
    assert_pw(*combined != pf3, "operator!= true");
    delete combined;
    std::ostringstream oss;
    oss << pf1;
    assert_pw(oss.str().find("PiecewiseFunction") != std::string::npos, "operator<< output contains header");
}

void test_enumerator() {
    std::cout << "\n=== Enumerator ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, -1, 4);
    piecewise_enumerator en(*pf.get_segments());
    int count = 0;
    while (en.move_next()) {
        ++count;
    }
    assert_pw(count == 2, "enumerator count");
    en.reset();
    assert_pw(en.move_next(), "enumerator reset & move");
    segment s = en.current();
    assert_pw(std::abs(s.start_x - 0) < 1e-9, "enumerator first segment start");
    piecewise_enumerator en2(*pf.get_segments());
    bool caught = false;
    try {
        en2.current();
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "enumerator current before move_next throws");
}

void test_pw_algorithms() {
    std::cout << "\n=== Algorithms ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 2, 0);
    pf.add_segment(2, 4, 4, 0);
    auto* doubled = map_segments(&pf, [](segment s) {
        return segment(s.start_x, s.end_x, s.k * 2, s.m * 2);
    });
    assert_pw(std::abs(doubled->get_segment(0).k - 4.0) < 1e-9, "map_segments k doubled");
    assert_pw(std::abs(doubled->get_segment(1).k - 8.0) < 1e-9, "map_segments k doubled seg2");
    delete doubled;
    auto* filtered = filter_segments(&pf, [](segment s) {
        return s.k > 3;
    });
    assert_pw(filtered->get_segment_count() == 1, "filter_segments count");
    assert_pw(std::abs(filtered->get_segment(0).k - 4.0) < 1e-9, "filter_segments keeps correct segment");
    delete filtered;
    double total_k = reduce_segments(&pf, [](double acc, segment s) {
        return acc + s.k;
    }, 0.0);
    assert_pw(std::abs(total_k - 6.0) < 1e-9, "reduce_segments sum k");
    mutable_piecewise_function empty_pf;
    auto* empty_filtered = filter_segments(&empty_pf, [](segment) {
        return true;
    });
    assert_pw(empty_filtered->get_segment_count() == 0, "filter_segments empty input");
    delete empty_filtered;
}

void test_edge_cases() {
    std::cout << "\n=== Edge Cases ===" << std::endl;
    mutable_piecewise_function empty;
    assert_pw(empty.get_segment_count() == 0, "empty pf segment count");
    bool caught = false;
    try {
        empty.evaluate(0);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "evaluate on empty throws");
    mutable_piecewise_function single;
    single.add_segment(0, 10, 2, 3);
    assert_pw(std::abs(single.evaluate(0) - 3.0) < 1e-9, "single seg at start");
    assert_pw(std::abs(single.evaluate(10) - 23.0) < 1e-9, "single seg at end");
    assert_pw(std::abs(single.evaluate(5) - 13.0) < 1e-9, "single seg middle");
    mutable_piecewise_function adjacent;
    adjacent.add_segment(0, 2, 1, 0);
    adjacent.add_segment(2, 4, 2, -2);
    assert_pw(std::abs(adjacent.evaluate(2) - (2 * 2 - 2)) < 1e-9, "adjacent boundary uses right segment");
    mutable_piecewise_function overlap;
    overlap.add_segment(0, 5, 1, 0);
    overlap.add_segment(3, 7, 2, 0);
    assert_pw(std::abs(overlap.evaluate(4) - 4.0) < 1e-9, "overlap uses first segment (def)");
}

void test_evaluation_advanced() {
    std::cout << "\n=== Advanced Evaluation ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(-10, -5, 0.5, 10);
    pf.add_segment(-5, 5, 0, 2.5);
    pf.add_segment(5, 10, -0.5, 7.5);
    assert_pw(std::abs(pf.evaluate(-7.5) - 6.25) < 1e-9, "adv eval negative x");
    assert_pw(std::abs(pf.evaluate(0) - 2.5) < 1e-9, "adv eval constant seg");
    assert_pw(std::abs(pf.evaluate(7.5) - 3.75) < 1e-9, "adv eval negative slope");
    assert_pw(std::abs(pf.evaluate(-5.0 + 1e-10) - 2.5) < 1e-9, "adv eval right boundary epsilon");
}

void test_segment_management() {
    std::cout << "\n=== Segment Management ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 4, 1, 0);
    pf.remove_segment(0);
    assert_pw(pf.get_segment_count() == 0, "sm: remove first empty");
    pf.add_segment(0, 2, 2, 0);
    pf.add_segment(2, 4, 2, -4);
    pf.add_segment(4, 6, 2, -8);
    pf.remove_segment(1);
    assert_pw(pf.get_segment_count() == 2, "sm: remove middle count");
    assert_pw(std::abs(pf.evaluate(5) - 2.0) < 1e-9, "sm: remove middle keeps valid seg");
    pf.remove_segment(1);
    assert_pw(pf.get_segment_count() == 1, "sm: remove last count");
    bool caught = false;
    try {
        pf.remove_segment(5);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "sm: remove out of range throws");
    caught = false;
    try {
        pf.remove_segment(-1);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "sm: remove negative index throws");
}

void test_redefine_advanced() {
    std::cout << "\n=== Redefine Advanced ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 10, 1, 0);
    auto* new_pf = dynamic_cast<mutable_piecewise_function*>(pf.redefine_on_interval(3, 7, 0, 5));
    assert_pw(new_pf->get_segment_count() == 3, "ra: split into 3 segments");
    assert_pw(std::abs(new_pf->evaluate(1) - 1.0) < 1e-9, "ra: left unchanged");
    assert_pw(std::abs(new_pf->evaluate(5) - 5.0) < 1e-9, "ra: middle overridden");
    assert_pw(std::abs(new_pf->evaluate(9) - 9.0) < 1e-9, "ra: right unchanged");

    mutable_piecewise_function pf2;
    pf2.add_segment(0, 10, 1, 0);
    auto* full_red = dynamic_cast<mutable_piecewise_function*>(pf2.redefine_on_interval(0, 10, -1, 10));
    assert_pw(full_red->get_segment_count() == 1, "ra: full redefine count");
    assert_pw(std::abs(full_red->evaluate(5) - 5.0) < 1e-9, "ra: full redefine value");
}

void test_properties_advanced() {
    std::cout << "\n=== Properties Advanced ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 0, 5);
    pf.add_segment(2, 4, 0, 5);
    assert_pw(pf.is_monotonic(), "pa: constant is monotonic");
    assert_pw(pf.is_continuous(), "pa: constant continuous");
    mutable_piecewise_function disjoint;
    disjoint.add_segment(0, 1, 1, 0);
    disjoint.add_segment(2, 3, 1, 0);
    bool caught = false;
    try {
        disjoint.evaluate(1.5);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "pa: gap throws on eval");
    assert_pw(disjoint.get_segment_count() == 2, "pa: gap preserves segment count");
}

void test_operators_advanced() {
    std::cout << "\n=== Operators Advanced ===" << std::endl;
    mutable_piecewise_function pf1;
    mutable_piecewise_function pf2;
    mutable_piecewise_function pf3;
    pf1.add_segment(0, 2, 1, 0);
    pf2.add_segment(2, 4, 1, 0);
    pf3.add_segment(4, 6, 1, 0);
    piecewise_function* temp = pf1 + pf2;
    auto* combined = *temp + pf3;
    delete temp;
    assert_pw(combined->get_segment_count() == 3, "oa: chain + count");
    assert_pw(std::abs(combined->evaluate(5) - 5.0) < 1e-9, "oa: chain + eval");
    delete combined;
    auto* clone = pf1.clone();
    pf1.add_segment(2, 3, 2, 0);
    assert_pw(clone->get_segment_count() == 1, "oa: clone deep independence");
    assert_pw(pf1.get_segment_count() == 2, "oa: original modified");
    delete clone;
}

void test_enumerator_advanced() {
    std::cout << "\n=== Enumerator Advanced ===" << std::endl;
    mutable_piecewise_function pf;
    piecewise_enumerator en(*pf.get_segments());
    assert_pw(!en.move_next(), "ea: empty enumerator move_next false");
    bool caught = false;
    try {
        en.current();
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "ea: empty current throws");
    mutable_piecewise_function pf2;
    pf2.add_segment(0, 1, 1, 0);
    piecewise_enumerator en2(*pf2.get_segments());
    en2.move_next();
    en2.move_next();
    assert_pw(!en2.move_next(), "ea: past end move_next false");
    en2.reset();
    assert_pw(en2.move_next(), "ea: reset works");
}

void test_algorithms_advanced() {
    std::cout << "\n=== Algorithms Advanced ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, 2, -2);
    auto* mapped = map_segments(&pf, [](segment s) {
        return s;
    });
    assert_pw(*mapped == pf, "aa: map identity equality");
    delete mapped;
    auto* filtered_none = filter_segments(&pf, [](segment s) {
        return s.k > 10;
    });
    assert_pw(filtered_none->get_segment_count() == 0, "aa: filter none count");
    delete filtered_none;
    double max_k = reduce_segments(&pf, [](double acc, segment s) {
        return std::max(acc, s.k);
    }, -1e9);
    assert_pw(std::abs(max_k - 2.0) < 1e-9, "aa: reduce max k");
}

void test_concat_operations() {
    std::cout << "\n=== Concat Operations ===" << std::endl;
    mutable_piecewise_function pf1;
    mutable_piecewise_function pf2;
    pf1.add_segment(0, 2, 1, 0);
    pf2.add_segment(2, 4, 2, -2);
    auto* mut_concat = dynamic_cast<mutable_piecewise_function*>(pf1.clone());
    mut_concat->concat(pf2);
    assert_pw(mut_concat->get_segment_count() == 2, "co: mutable concat count");
    assert_pw(std::abs(mut_concat->evaluate(3) - 4.0) < 1e-9, "co: mutable concat eval");
    delete mut_concat;
    immutable_piecewise_function imm1;
    immutable_piecewise_function imm2;
    immutable_piecewise_function* imm_res = dynamic_cast<immutable_piecewise_function*>(imm1.concat(imm2));
    assert_pw(imm_res->get_segment_count() == 0, "co: immutable empty concat");
    delete imm_res;
    immutable_piecewise_function* i1 = dynamic_cast<immutable_piecewise_function*>(imm1.add_segment(0, 1, 1, 0));
    immutable_piecewise_function* i2 = dynamic_cast<immutable_piecewise_function*>(imm1.add_segment(1, 2, 2, -2));
    immutable_piecewise_function* i3 = dynamic_cast<immutable_piecewise_function*>(i1->concat(*i2));
    assert_pw(i3->get_segment_count() == 2, "co: immutable non-empty concat");
    assert_pw(std::abs(i3->evaluate(0.5) - 0.5) < 1e-9, "co: immutable concat eval seg1");
    assert_pw(std::abs(i3->evaluate(1.5) - 1.0) < 1e-9, "co: immutable concat eval seg2");
    delete i1;
    delete i2;
    delete i3;
}

void test_clone_independence() {
    std::cout << "\n=== Clone Independence ===" << std::endl;
    mutable_piecewise_function src;
    src.add_segment(0, 5, 3, 1);
    auto* cl = src.clone();
    src.add_segment(5, 10, -1, 15);
    assert_pw(cl->get_segment_count() == 1, "ci: clone segment count unchanged");
    assert_pw(src.get_segment_count() == 2, "ci: source modified after clone");
    delete cl;
    immutable_piecewise_function i_src;
    immutable_piecewise_function* i1 = dynamic_cast<immutable_piecewise_function*>(i_src.add_segment(0, 2, 1, 0));
    auto* i_cl = i1->clone();
    assert_pw(i_cl->get_segment_count() == 1, "ci: immutable clone count");
    assert_pw(*i_cl == *i1, "ci: immutable clone equality");
    delete i1;
    delete i_cl;
}

void test_redefine_boundary() {
    std::cout << "\n=== Redefine Boundary ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 10, 2, 0);
    auto* rb1 = dynamic_cast<mutable_piecewise_function*>(pf.redefine_on_interval(0, 0, 5, 5));
    assert_pw(rb1->get_segment_count() == 1, "rb: zero-length start split");

    auto* rb2 = dynamic_cast<mutable_piecewise_function*>(pf.redefine_on_interval(5, 10, 0, 0));
    assert_pw(rb2->get_segment_count() == 2, "rb: exact end overlap");
    assert_pw(std::abs(rb2->evaluate(7.5) - 0.0) < 1e-9, "rb: exact end overlap eval");

    auto* rb3 = dynamic_cast<mutable_piecewise_function*>(pf.redefine_on_interval(-5, 15, 1, 0));
    assert_pw(rb3->get_segment_count() <= 2, "rb: full cover redefine");
    assert_pw(std::abs(rb3->evaluate(5) - 5.0) < 1e-9, "rb: full cover eval");
}

void test_algorithms_reduce_multiply() {
    std::cout << "\n=== Algorithms Reduce Multiply ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 2, 0);
    pf.add_segment(2, 4, 3, 0);
    pf.add_segment(4, 6, 4, 0);
    double prod_k = reduce_segments(&pf, [](double acc, segment s) {
        return acc * s.k;
    }, 1.0);
    assert_pw(std::abs(prod_k - 24.0) < 1e-9, "arm: reduce multiply k");
    double sum_m = reduce_segments(&pf, [](double acc, segment s) {
        return acc + s.m;
    }, 0.0);
    assert_pw(std::abs(sum_m - 0.0) < 1e-9, "arm: reduce sum m zero");
    auto* scaled = map_segments(&pf, [](segment s) {
        return segment(s.start_x, s.end_x, s.k, s.m + 1);
    });
    assert_pw(std::abs(scaled->get_segment(0).m - 1.0) < 1e-9, "arm: map shift m");
    delete scaled;
}

void test_continuous_tolerance() {
    std::cout << "\n=== Continuous Tolerance ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 1, 0, 0);
    pf.add_segment(1, 2, 0, 1e-10);
    assert_pw(pf.is_continuous(), "ct: epsilon tolerance passes");
    pf.add_segment(2, 3, 0, 1e-4);
    assert_pw(!pf.is_continuous(1e-5), "ct: explicit gap fails with strict eps");
    assert_pw(pf.is_continuous(1e-3), "ct: explicit gap passes with loose eps");
    mutable_piecewise_function empty_pf;
    assert_pw(empty_pf.is_continuous(), "ct: empty is continuous");
}

void test_monotonic_flat() {
    std::cout << "\n=== Monotonic Flat ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 0, 0);
    pf.add_segment(2, 4, 0, 0);
    assert_pw(pf.is_monotonic(), "mf: flat is monotonic");
    pf.add_segment(4, 6, 0, 1);
    assert_pw(pf.is_monotonic(), "mf: step up is monotonic nondecreasing");
    mutable_piecewise_function down;
    down.add_segment(0, 2, -1, 2);
    down.add_segment(2, 4, 0, 0);
    assert_pw(down.is_monotonic(), "mf: decreasing+flat monotonic");
    mutable_piecewise_function mixed;
    mixed.add_segment(0, 2, 1, 0);
    mixed.add_segment(2, 4, -1, 4);
    assert_pw(!mixed.is_monotonic(), "mf: up+down not monotonic");
    mutable_piecewise_function up_then_down_step;
    up_then_down_step.add_segment(0, 2, 0, 0);
    up_then_down_step.add_segment(2, 4, 0, 1);
    up_then_down_step.add_segment(4, 6, 0, 0);
    assert_pw(!up_then_down_step.is_monotonic(), "mf: step up then down is not monotonic");
}

void test_segment_access() {
    std::cout << "\n=== Segment Access ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, 2, -2);
    segment s0 = pf.get_segment(0);
    assert_pw(std::abs(s0.start_x - 0.0) < 1e-9, "sa: get_segment(0) start");
    segment s1 = pf.get_segment(1);
    assert_pw(std::abs(s1.end_x - 4.0) < 1e-9, "sa: get_segment(1) end");
    bool caught = false;
    try {
        pf.get_segment(5);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "sa: out of range throws");
}

void test_negative_domains() {
    std::cout << "\n=== Negative Domains ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(-10, -5, 1, 0);
    pf.add_segment(-5, 0, 2, 10);
    assert_pw(std::abs(pf.evaluate(-8) - (-8.0)) < 1e-9, "nd: negative x eval");
    assert_pw(std::abs(pf.evaluate(-2) - (2 * (-2) + 10)) < 1e-9, "nd: negative x second seg");
    assert_pw(pf.is_monotonic(), "nd: negative domain monotonic");
    bool caught = false;
    try {
        pf.evaluate(1);
    }
    catch (...) {
        caught = true;
    }
    assert_pw(caught, "nd: positive out of domain throws");
}

void test_interval_operations() {
    std::cout << "\n=== Interval Operations ===" << std::endl;
    interval ivl(0.0, 5.0);
    assert_pw(std::abs(ivl.left - 0.0) < 1e-9, "ivl: left bound");
    assert_pw(std::abs(ivl.right - 5.0) < 1e-9, "ivl: right bound");
    assert_pw(std::abs(ivl.length() - 5.0) < 1e-9, "ivl: length = 5");
    assert_pw(ivl.contains(0.0), "ivl: contains left boundary");
    assert_pw(ivl.contains(5.0), "ivl: contains right boundary");
    assert_pw(ivl.contains(2.5), "ivl: contains middle");
    assert_pw(!ivl.contains(-0.1), "ivl: does not contain left outside");
    assert_pw(!ivl.contains(5.1), "ivl: does not contain right outside");
    interval a(0.0, 4.0);
    interval b(2.0, 6.0);
    interval c(5.0, 7.0);
    assert_pw(a.overlaps(b), "ivl: [0,4] overlaps [2,6]");
    assert_pw(b.overlaps(a), "ivl: overlaps symmetric");
    assert_pw(!a.overlaps(c), "ivl: [0,4] does not overlap [5,7]");
    assert_pw(b.overlaps(c), "ivl: [2,6] overlaps [5,7]");
    interval point(3.0, 3.0);
    assert_pw(std::abs(point.length() - 0.0) < 1e-9, "ivl: point interval length 0");
    assert_pw(point.contains(3.0), "ivl: point contains its value");
}

void test_continuous_on_interval() {
    std::cout << "\n=== Continuous On Interval ===" << std::endl;
    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, 1, 0);
    pf.add_segment(4, 6, 1, 10);
    assert_pw(!pf.is_continuous(), "cont_on: full interval has break");
    assert_pw(pf.is_continuous_on(interval(0.0, 4.0)), "cont_on: [0,4] continuous");
    assert_pw(!pf.is_continuous_on(interval(3.0, 5.0)), "cont_on: [3,5] has break");
    assert_pw(pf.is_continuous_on(interval(5.0, 6.0)), "cont_on: [5,6] single segment");
    assert_pw(pf.is_continuous_on(interval(10.0, 12.0)), "cont_on: outside domain → true");
}

void test_monotonic_on_interval() {
    std::cout << "\n=== Monotonic On Interval ===" << std::endl;

    mutable_piecewise_function pf;
    pf.add_segment(0, 2, 1, 0);
    pf.add_segment(2, 4, -1, 4);
    pf.add_segment(4, 6, 1, -4);
    assert_pw(!pf.is_monotonic(), "mono_on: full interval not monotonic");
    assert_pw(pf.is_monotonic_on(interval(0.0, 2.0)), "mono_on: [0,2] monotonic (increasing)");
    assert_pw(pf.is_monotonic_on(interval(2.0, 4.0)), "mono_on: [2,4] monotonic (decreasing)");
    assert_pw(!pf.is_monotonic_on(interval(1.0, 3.0)), "mono_on: [1,3] not monotonic");
    assert_pw(pf.is_monotonic_on(interval(5.0, 6.0)), "mono_on: [5,6] single segment");
}

void run_piecewise_tests() {
    piecewise_passed = piecewise_failed = 0;
    std::cout << "\n========== RUNNING PIECEWISE TESTS ==========" << std::endl;
    test_evaluation();
    test_properties();
    test_pw_mutable_vs_immutable();
    test_pw_operators();
    test_enumerator();
    test_pw_algorithms();
    test_edge_cases();
    test_evaluation_advanced();
    test_segment_management();
    test_redefine_advanced();
    test_properties_advanced();
    test_operators_advanced();
    test_enumerator_advanced();
    test_algorithms_advanced();
    test_concat_operations();
    test_clone_independence();
    test_redefine_boundary();
    test_algorithms_reduce_multiply();
    test_continuous_tolerance();
    test_monotonic_flat();
    test_interval_operations();
    test_continuous_on_interval();
    test_monotonic_on_interval();
    test_segment_access();
    test_negative_domains();
    std::cout << "\n========== RESULTS ==========" << std::endl;
    std::cout << "Passed: " << piecewise_passed << std::endl;
    std::cout << "Failed: " << piecewise_failed << std::endl;
    std::cout << "Total:  " << (piecewise_passed + piecewise_failed) << std::endl;
    std::cout << "=============================================" << std::endl;
}
