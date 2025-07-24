#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Testbench.h"

/* === Fixed Format === */
Testbench::Testbench(sc_module_name n)
    : sc_module(n) {
    SC_THREAD(do_feed);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(do_fetch);
    sensitive << i_clk.pos();
    dont_initialize();
}
/* === Fixed Format End === */

void Testbench::do_feed() {
    struct TestCase {
        double train[NUM_TRAIN][DIM];
        int labels[NUM_TRAIN];
        double query_point[DIM];
        int expected;
    };
    std::vector<TestCase> tests;
    std::ifstream fin("testcases.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open testcases.txt\n";
        sc_stop();
        return;
    }
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        TestCase tc;
        for (int i = 0; i < NUM_TRAIN; i++) {
            for (int d = 0; d < DIM; d++) {
                if (!(iss >> tc.train[i][d])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
        }
        for (int i = 0; i < NUM_TRAIN; i++) {
            if (!(iss >> tc.labels[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                goto skip_testcase;
            }
        }
        for (int d = 0; d < DIM; d++) {
            if (!(iss >> tc.query_point[d])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                goto skip_testcase;
            }
        }
        if (!(iss >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            goto skip_testcase;
        }
        tests.push_back(tc);
skip_testcase:
        continue;
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        TestCase& tc = tests[idx];
        for (int i = 0; i < NUM_TRAIN; i++) {
            for (int d = 0; d < DIM; d++) {
                o_train.write(tc.train[i][d]);
            }
        }
        for (int i = 0; i < NUM_TRAIN; i++) {
            o_labels.write(tc.labels[i]);
        }
        for (int d = 0; d < DIM; d++) {
            o_query_point.write(tc.query_point[d]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<int> results;
    wait(1);

    for (size_t idx = 0; idx < results.size(); ++idx) {
        int result = i_result.read();
        results.push_back(result);
    }

    std::ifstream fin("golden.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open golden.txt\n";
        sc_stop();
        return;
    }
    std::vector<int> goldens;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        int expected;
        if (!(iss >> expected)) {
            std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
            continue;
        }
        goldens.push_back(expected);
    }
    fin.close();

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result = results[idx];
        bool passed = (result == goldens[idx]);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully.\n";
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
