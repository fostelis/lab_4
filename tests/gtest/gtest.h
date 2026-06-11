#ifndef MINI_GTEST_COMPAT_H
#define MINI_GTEST_COMPAT_H

#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace testing {

struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> body;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& failed_count() {
    static int value = 0;
    return value;
}

inline int& passed_count() {
    static int value = 0;
    return value;
}

class TestRegistrar {
public:
    TestRegistrar(const char* suite, const char* name, std::function<void()> body) {
        registry().push_back({suite, name, body});
    }
};

inline void InitGoogleTest(int*, char**) {}

inline int RUN_ALL_TESTS() {
    failed_count() = 0;
    passed_count() = 0;
    for (const auto& test : registry()) {
        int before = failed_count();
        std::cout << "[ RUN      ] " << test.suite << "." << test.name << std::endl;
        try {
            test.body();
        } catch (const std::exception& ex) {
            ++failed_count();
            std::cout << "[  ERROR   ] " << ex.what() << std::endl;
        } catch (...) {
            ++failed_count();
            std::cout << "[  ERROR   ] unknown exception" << std::endl;
        }
        if (failed_count() == before) {
            ++passed_count();
            std::cout << "[       OK ] " << test.suite << "." << test.name << std::endl;
        } else {
            std::cout << "[  FAILED  ] " << test.suite << "." << test.name << std::endl;
        }
    }
    std::cout << "\n========== RESULTS ==========" << std::endl;
    std::cout << "Passed: " << passed_count() << std::endl;
    std::cout << "Failed: " << failed_count() << std::endl;
    std::cout << "Total:  " << passed_count() + failed_count() << std::endl;
    std::cout << "=============================" << std::endl;
    return failed_count() == 0 ? 0 : 1;
}

inline void expect_true(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        ++failed_count();
        std::cout << file << ":" << line << ": Failure\nExpected true: " << expression << std::endl;
    }
}

}

#define TEST(SUITE, NAME) \
    static void SUITE##_##NAME##_impl(); \
    static ::testing::TestRegistrar SUITE##_##NAME##_registrar(#SUITE, #NAME, SUITE##_##NAME##_impl); \
    static void SUITE##_##NAME##_impl()

#define EXPECT_TRUE(CONDITION) ::testing::expect_true(static_cast<bool>(CONDITION), #CONDITION, __FILE__, __LINE__)
#define EXPECT_FALSE(CONDITION) ::testing::expect_true(!(CONDITION), "!(" #CONDITION ")", __FILE__, __LINE__)
#define EXPECT_EQ(A, B) ::testing::expect_true(((A) == (B)), #A " == " #B, __FILE__, __LINE__)
#define EXPECT_NE(A, B) ::testing::expect_true(((A) != (B)), #A " != " #B, __FILE__, __LINE__)

#define EXPECT_THROW(STATEMENT, EXCEPTION_TYPE) \
    do { \
        bool gtest_compat_caught = false; \
        try { STATEMENT; } catch (const EXCEPTION_TYPE&) { gtest_compat_caught = true; } catch (...) {} \
        ::testing::expect_true(gtest_compat_caught, #STATEMENT " throws " #EXCEPTION_TYPE, __FILE__, __LINE__); \
    } while (false)

#define EXPECT_NO_THROW(STATEMENT) \
    do { \
        bool gtest_compat_caught = false; \
        try { STATEMENT; } catch (...) { gtest_compat_caught = true; } \
        ::testing::expect_true(!gtest_compat_caught, #STATEMENT " does not throw", __FILE__, __LINE__); \
    } while (false)

#endif
