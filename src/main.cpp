#include <iostream>
#include <vector>
#include "dynamic_array.h"
#include "linked_list.h"
#include "array_sequence.h"
#include "list_sequence.h"
#include "bit_sequence.h"
#include "algorithms.h"
#include "tests.h"
#include "io_utils.h"

#include "piecewise_function.h"
#include "mutable_piecewise_function.h"
#include "immutable_piecewise_function.h"
#include "piecewise_enumerator.h"
#include "piecewise_operators.h"
#include "piecewise_algorithms.h"
#include "lazy_sequence.h"
#include "streams.h"
#include "online_statistics.h"
#include "lab4_format.h"
#include "ordinal_operations.h"

using namespace std;

void run_piecewise_tests();

void run_lab_2();
void run_lab_3();
void run_lab_4();
void run_lab4_tests();


void print_menu_lab_2();
void test_dynamic_array_ui();
void test_linked_list_ui();
void test_array_sequence_ui();
void test_immutable_ui();
void test_bit_ui();
void test_operators_ui();
void test_extended_ui();
void test_algorithms_ui();
void test_crtp_demo();

int main() {
    setlocale(LC_ALL, "Russian");
    int main_choice;
    do {
        cout << "\n========================================" << endl;
        cout << "    MAIN MENU: Select Lab" << endl;
        cout << "========================================" << endl;
        cout << "1. Lab 2 (Sequence, Arrays, Lists)" << endl;
        cout << "2. Lab 3 (Piecewise Function)" << endl;
        cout << "3. Lab 4 (LazySequence, Streams, Online Statistics)" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> main_choice;
        switch (main_choice) {
            case 1:
                run_lab_2();
                break;
            case 2:
                run_lab_3();
                break;
            case 3:
                run_lab_4();
                break;
            case 0:
                cout << "Exiting program..." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    } while (main_choice != 0);
    return 0;
}

void run_lab_2() {
    int choice;
    do {
        print_menu_lab_2();
        cin >> choice;
        switch (choice) {
            case 1:
                test_dynamic_array_ui();
                break;
            case 2:
                test_linked_list_ui();
                break;
            case 3:
                test_array_sequence_ui();
                break;
            case 4:
                test_immutable_ui();
                break;
            case 5:
                test_bit_ui();
                break;
            case 6:
                test_operators_ui();
                break;
            case 7:
                test_extended_ui();
                break;
            case 8:
                test_algorithms_ui();
                break;
            case 9:
                run_all_tests();
                break;
            case 10:
                test_crtp_demo();
                break;
            case 0:
                cout << "Back to main menu..." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}

void print_menu_lab_2() {
    cout << "\n=== LAB 2 MENU ===" << endl;
    cout << "1. DynamicArray" << endl;
    cout << "2. LinkedList" << endl;
    cout << "3. MutableArraySequence" << endl;
    cout << "4. ImmutableArraySequence" << endl;
    cout << "5. BitSequence" << endl;
    cout << "6. Operators" << endl;
    cout << "7. Extended Map-Reduce" << endl;
    cout << "8. Algorithms" << endl;
    cout << "9. Run All Tests (Lab 2)" << endl;
    cout << "10. CRTP Demo" << endl;
    cout << "0. Back" << endl;
    cout << "Choice: ";
}

void run_lab_3() {
    int choice;
    do {
        cout << "\n=== LAB 3 MENU (Piecewise Function) ===" << endl;
        cout << "1. Run Piecewise Function Tests" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                run_piecewise_tests();
                break;
            case 0:
                cout << "Back to main menu..." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}


void test_dynamic_array_ui() {
    cout << "\n=== DynamicArray ===" << endl;
    int arr[] = {1, 2, 3, 4, 5};
    dynamic_array<int> da(arr, 5);
    cout << "Values: ";
    for (int i = 0; i < da.get_size(); ++i) {
        cout << da.get(i) << " ";
    }
    cout << endl;
}

void test_linked_list_ui() {
    cout << "\n=== LinkedList ===" << endl;
    linked_list<int> list;
    list.append(1);
    list.append(2);
    list.prepend(0);
    cout << "Values: ";
    for (int i = 0; i < list.get_length(); ++i) {
        cout << list.get(i) << " ";
    }
    cout << endl;
}

void test_array_sequence_ui() {
    cout << "\n=== MutableArraySequence ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    seq->append(1);
    seq->append(2);
    cout << "Values: " << *seq << endl;
    delete seq;
}

void test_immutable_ui() {
    cout << "\n=== ImmutableArraySequence ===" << endl;
    immutable_array_sequence<int>* seq = new immutable_array_sequence<int>();
    seq = dynamic_cast<immutable_array_sequence<int>*>(seq->append(1));
    seq = dynamic_cast<immutable_array_sequence<int>*>(seq->append(2));
    cout << "Values: " << *seq << endl;
    delete seq;
}

void test_bit_ui() {
    cout << "\n=== BitSequence ===" << endl;
    bit_sequence* bs = new bit_sequence();
    bs->append(bit(true));
    bs->append(bit(false));
    bs->append(bit(true));
    cout << "Bits: ";
    for (int i = 0; i < bs->get_length(); ++i) {
        cout << (bs->get(i).get_value() ? "1" : "0");
    }
    cout << endl;
    delete bs;
}

void test_operators_ui() {
    cout << "\n=== Operators ===" << endl;
    mutable_array_sequence<int>* s1 = new mutable_array_sequence<int>();
    mutable_array_sequence<int>* s2 = new mutable_array_sequence<int>();
    s1->append(1);
    s2->append(2);
    cout << "Equal: " << (*s1 == *s1 ? "yes" : "no") << endl;
    sequence<int>* concat = *s1 + *s2;
    cout << "Concat: " << *concat << endl;
    delete concat;
    delete s1;
    delete s2;
}

void test_extended_ui() {
    cout << "\n=== Extended Map-Reduce ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    cout << "Original: " << *seq << endl;
    sequence<int>* skipped = seq->skip(2);
    cout << "Skip(2): " << *skipped << endl;
    delete skipped;
    delete seq;
}

void test_algorithms_ui() {
    cout << "\n=== Algorithms ===" << endl;
    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 1; i <= 5; ++i) {
        seq->append(i);
    }
    stats<int> st = get_stats(*seq);
    cout << "Stats: min=" << st.min_val << ", max=" << st.max_val << ", avg=" << st.avg_val << endl;
    delete seq;
}

void test_crtp_demo() {
    cout << "\n=== CRTP Demo ===" << endl;
    cout << "Demonstrating Curiously Recurring Template Pattern" << endl;
    mutable_array_sequence_crtp<int>* seq = new mutable_array_sequence_crtp<int>();
    seq->append(10);
    seq->append(20);
    cout << "Mutable CRTP: " << *seq << endl;
    immutable_array_sequence_crtp<int>* imm = new immutable_array_sequence_crtp<int>();
    imm = dynamic_cast<immutable_array_sequence_crtp<int>*>(imm->append(100));
    cout << "Immutable CRTP: " << *imm << endl;
    delete seq;
    delete imm;
    cout << "[INFO] CRTP allows compile-time polymorphism instead of virtual functions" << endl;
}

static int read_int_value(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

static void print_sequence_values(const sequence<int>& values) {
    for (int i = 0; i < values.get_length(); ++i) cout << values.get(i) << " ";
    cout << endl;
}

static int fibonacci_ui_rule(const sequence<int>& values, size_t) {
    return values.get(values.get_length() - 1) + values.get(values.get_length() - 2);
}

static int natural_ui_rule(const sequence<int>&, size_t index) {
    return static_cast<int>(index);
}

static int multiply_by_two_ui(const int& value) {
    return value * 2;
}

static int square_ui_value(const int& value) {
    return value * value;
}

static bool is_even_ui(const int& value) {
    return value % 2 == 0;
}

class shifted_natural_ui_rule {
private:
    int shift_;
public:
    explicit shifted_natural_ui_rule(int shift) : shift_(shift) {}

    int operator()(const sequence<int>&, size_t index) const {
        return shift_ + static_cast<int>(index);
    }
};

static lazy_sequence<int> make_lazy_fibonacci_ui() {
    int seed_items[] = {1, 1};
    mutable_array_sequence<int> seed(seed_items, 2);
    return lazy_sequence<int>(fibonacci_ui_rule, seed);
}

static lazy_sequence<int> make_lazy_naturals_ui() {
    int seed_items[] = {0};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(natural_ui_rule, seed);
}

static void lab4_demo_infinite_fibonacci() {
    auto fib = make_lazy_fibonacci_ui();
    cout << "\nDemo: infinite Fibonacci LazySequence" << endl;
    cout << "Length: omega/infinity = " << (fib.get_length().has_omega_part() ? "yes" : "no") << endl;
    cout << "Before reading, materialized elements: " << fib.get_materialized_count() << endl;
    int n = read_int_value("How many first elements to show? ");
    if (n < 0) n = 0;
    cout << "First " << n << " elements: ";
    print_sequence_values(fib.take(static_cast<size_t>(n)));
    cout << "After reading, materialized elements: " << fib.get_materialized_count() << endl;
}

static void lab4_manual_lazy_sequence() {
    cout << "\nManual LazySequence test" << endl;
    cout << "1. Finite sequence from keyboard" << endl;
    cout << "2. Infinite naturals: 0, 1, 2, 3, ..." << endl;
    cout << "3. Infinite Fibonacci" << endl;
    int mode = read_int_value("Choice: ");

    if (mode == 1) {
        int n = read_int_value("Enter count: ");
        if (n < 0) {
            cout << "Count must be non-negative." << endl;
            return;
        }
        mutable_array_sequence<int> data;
        for (int i = 0; i < n; ++i) data.append(read_int_value("Element: "));
        lazy_sequence<int> seq(data);
        cout << "Sequence length: " << seq.get_length().finite_value() << endl;
        cout << "Materialized count: " << seq.get_materialized_count() << endl;
        cout << "Operations: get / append / prepend / insert / map(*2) / where(even)" << endl;
        int op = read_int_value("1-get, 2-append, 3-prepend, 4-insert, 5-map, 6-where: ");
        try {
            if (op == 1) {
                int index = read_int_value("Index: ");
                cout << "Value = " << seq.get(index) << endl;
            } else if (op == 2) {
                int value = read_int_value("Value to append: ");
                auto res = seq.append(value);
                cout << "Result: "; print_sequence_values(res.take(res.get_length().finite_value()));
            } else if (op == 3) {
                int value = read_int_value("Value to prepend: ");
                auto res = seq.prepend(value);
                cout << "Result: "; print_sequence_values(res.take(res.get_length().finite_value()));
            } else if (op == 4) {
                int index = read_int_value("Index: ");
                int value = read_int_value("Value: ");
                auto res = seq.insert_at(value, index);
                cout << "Result: "; print_sequence_values(res.take(res.get_length().finite_value()));
            } else if (op == 5) {
                auto res = seq.map<int>(multiply_by_two_ui);
                cout << "Result: "; print_sequence_values(res.take(res.get_length().finite_value()));
            } else if (op == 6) {
                auto res = seq.where(is_even_ui);
                cout << "Result: "; print_sequence_values(res.take(res.get_length().finite_value()));
            }
        } catch (const exception& ex) {
            cout << "Error: " << ex.what() << endl;
        }
    } else {
        lazy_sequence<int> seq = (mode == 2) ? make_lazy_naturals_ui() : make_lazy_fibonacci_ui();
        cout << "Infinite sequence created. Append is allowed as omega+finite tail; get_last exists only if such tail exists." << endl;
        cout << "Initially materialized: " << seq.get_materialized_count() << endl;
        int n = read_int_value("How many elements to materialize/show? ");
        if (n < 0) n = 0;
        cout << "Values: "; print_sequence_values(seq.take(static_cast<size_t>(n)));
        cout << "Now materialized: " << seq.get_materialized_count() << endl;
    }
}

static void lab4_manual_stream_statistics() {
    cout << "\nManual ReadOnlyStream + online statistics" << endl;
    int n = read_int_value("Enter count of numbers: ");
    if (n < 0) {
        cout << "Count must be non-negative." << endl;
        return;
    }
    mutable_array_sequence<double> seq;
    for (int i = 0; i < n; ++i) {
        int value = read_int_value("Number: ");
        seq.append(static_cast<double>(value));
    }
    read_only_stream<double> stream(seq);
    auto stats = collect_statistics(stream);
    cout << "Statistics: " << stats << endl;
}

static void lab4_prepared_demo_scenarios() {
    cout << "\nPrepared test data / demo scenarios" << endl;
    cout << "1) Fibonacci first 12" << endl;
    auto fib = make_lazy_fibonacci_ui();
    cout << "   "; print_sequence_values(fib.take(12));
    cout << "   materialized = " << fib.get_materialized_count() << endl;

    cout << "2) Naturals mapped to squares, first 10" << endl;
    auto squares = make_lazy_naturals_ui().map<int>(square_ui_value);
    cout << "   "; print_sequence_values(squares.take(10));
    cout << "   materialized squares = " << squares.get_materialized_count() << endl;

    cout << "3) Prepared stream statistics" << endl;
    double values[] = {5, 1, 3, 2, 4, 10, -1};
    mutable_array_sequence<double> seq(values, 7);
    read_only_stream<double> stream(seq);
    cout << "   " << collect_statistics(stream) << endl;
}

static void lab4_stress_demo() {
    cout << "\nStress demo" << endl;
    int n = read_int_value("How many natural numbers to generate? Recommended 100000: ");
    if (n < 0) n = 0;
    auto naturals = make_lazy_naturals_ui();
    int last = (n == 0) ? -1 : naturals.get(n - 1);
    cout << "Last generated value: " << last << endl;
    cout << "Materialized count: " << naturals.get_materialized_count() << endl;
}


static ordinal_number read_ordinal_number_ui() {
    cout << "Enter omega block (0 for ordinary finite index, 1 for omega+..., 2 for omega*2+..., etc.): ";
    int block;
    cin >> block;
    if (block < 0) block = 0;
    int offset = read_int_value("Enter finite offset inside this block: ");
    if (offset < 0) offset = 0;
    return ordinal_number(static_cast<size_t>(block), static_cast<size_t>(offset));
}

static lazy_sequence<int> make_shifted_naturals_ui(int shift) {
    int seed_items[] = {shift};
    mutable_array_sequence<int> seed(seed_items, 1);
    return lazy_sequence<int>(shifted_natural_ui_rule(shift), seed);
}

static void lab4_flexible_ordinal_playground() {
    cout << "\nFlexible ordinal LazySequence playground" << endl;
    cout << "This mode demonstrates omega blocks: omega*0+i, omega*1+j, omega*2+k, ..." << endl;
    int count = read_int_value("How many infinite natural sequences to concatenate? ");
    if (count <= 0) {
        cout << "Count must be positive." << endl;
        return;
    }

    lazy_sequence<int> current = make_shifted_naturals_ui(0);
    for (int i = 1; i < count; ++i) {
        current = current.concat(make_shifted_naturals_ui(i * 1000));
    }

    cout << "Created sequence length: " << current.get_length() << endl;
    cout << "Sequence #0 is 0,1,2,..." << endl;
    cout << "Sequence #1 is 1000,1001,1002,..." << endl;
    cout << "Sequence #2 is 2000,2001,2002,... and so on." << endl;

    int op;
    do {
        cout << "\nFlexible commands" << endl;
        cout << "1. Get element by ordinal index omega*block+offset" << endl;
        cout << "2. Append finite element to the end" << endl;
        cout << "3. Prepend finite element to the beginning" << endl;
        cout << "4. Insert finite element by ordinal index" << endl;
        cout << "5. Concat one more infinite sequence" << endl;
        cout << "6. Map current sequence (*2)" << endl;
        cout << "7. Show length" << endl;
        cout << "8. Try get_last" << endl;
        cout << "9. Replace current sequence by round-robin interleave of N infinite sequences" << endl;
        cout << "0. Back" << endl;
        op = read_int_value("Choice: ");
        try {
            if (op == 1) {
                ordinal_number index = read_ordinal_number_ui();
                cout << "seq[" << index << "] = " << current.get(index) << endl;
            }
            else if (op == 2) {
                int value = read_int_value("Value to append: ");
                current = current.append(value);
                cout << "Appended. New length: " << current.get_length() << endl;
                if (ordinal_operations::has_last(current.get_length())) {
                    cout << "Last element: " << current.get_last() << endl;
                }
            }
            else if (op == 3) {
                int value = read_int_value("Value to prepend: ");
                current = current.prepend(value);
                cout << "Prepended. New length: " << current.get_length() << endl;
                cout << "First element: " << current.get_first() << endl;
            }
            else if (op == 4) {
                int value = read_int_value("Value to insert: ");
                ordinal_number index = read_ordinal_number_ui();
                current = current.insert_at(value, index);
                cout << "Inserted. New length: " << current.get_length() << endl;
                cout << "Inserted place now contains: " << current.get(index) << endl;
            }
            else if (op == 5) {
                int shift = read_int_value("Shift for new infinite sequence, e.g. 3000: ");
                current = current.concat(make_shifted_naturals_ui(shift));
                cout << "Concatenated. New length: " << current.get_length() << endl;
            }
            else if (op == 6) {
                current = current.map<int>(multiply_by_two_ui);
                cout << "Mapped. Length is still: " << current.get_length() << endl;
            }
            else if (op == 7) {
                cout << "Length: " << current.get_length() << endl;
            }
            else if (op == 8) {
                cout << "Last: " << current.get_last() << endl;
            }
            else if (op == 9) {
                int interleave_count = read_int_value("How many infinite sequences to interleave? ");
                if (interleave_count <= 0) {
                    cout << "Count must be positive." << endl;
                }
                else {
                    mutable_array_sequence<lazy_sequence<int>> sequences;
                    for (int i = 0; i < interleave_count; ++i) {
                        sequences.append(make_shifted_naturals_ui(i * 1000));
                    }
                    current = lazy_sequence<int>::interleave(sequences);
                    cout << "Interleaved. New length: " << current.get_length() << endl;
                    cout << "Example: 0, 1000, 2000, ..., 1, 1001, 2001, ..." << endl;
                }
            }
        }
        catch (const exception& ex) {
            cout << "Error: " << ex.what() << endl;
        }
    } while (op != 0);
}

void run_lab_4() {
    int choice;
    do {
        cout << "\n=== LAB 4 MENU (LazySequence / Streams) ===" << endl;
        cout << "1. Run Lab 4 unit tests" << endl;
        cout << "2. Manual LazySequence test (keyboard input)" << endl;
        cout << "3. Manual stream statistics (keyboard input)" << endl;
        cout << "4. Demo: infinite Fibonacci LazySequence" << endl;
        cout << "5. Prepared demo scenarios" << endl;
        cout << "6. Stress demo for large lazy sequence" << endl;
        cout << "7. Flexible ordinal concat/insert playground" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                run_lab4_tests();
                break;
            case 2:
                lab4_manual_lazy_sequence();
                break;
            case 3:
                lab4_manual_stream_statistics();
                break;
            case 4:
                lab4_demo_infinite_fibonacci();
                break;
            case 5:
                lab4_prepared_demo_scenarios();
                break;
            case 6:
                lab4_stress_demo();
                break;
            case 7:
                lab4_flexible_ordinal_playground();
                break;
            case 0:
                cout << "Back to main menu..." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}
