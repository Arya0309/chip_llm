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
    struct Testcase {
        std::vector<double> train;
        std::vector<int> labels;
        std::vector<double> query_point;
        int expected;
    };

    std::vector<Testcase> tests;
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
        /* === Variable Section === */
        Testcase tc;
        int num_train, dim, k;
        if (!(iss >> num_train >> dim >> k)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tc.train.resize(num_train * dim);
        tc.labels.resize(num_train);
        tc.query_point.resize(dim);
        for (int i = 0; i < num_train; ++i) {
            for (int d = 0; d < dim; ++d) {
                if (!(iss >> tc.train[i * dim + d])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    continue;
                }
            }
            if (!(iss >> tc.labels[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int d = 0; d < dim; ++d) {
            if (!(iss >> tc.query_point[d])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        /* === Variable Section === */
        const Testcase& tc = tests[idx];
        /* === Variable Section End === */

        /* === Variable Section === */
        for (const auto& val : tc.train) {
            o_train.write(val);
        }
        for (const auto& val : tc.labels) {
            o_labels.write(val);
        }
        for (const auto& val : tc.query_point) {
            o_query_point.write(val);
        }
        /* === Variable Section End === */

        wait();
    }
}

void Testbench::do_fetch() {
    struct Testcase {
        std::vector<double> train;
        std::vector<int> labels;
        std::vector<double> query_point;
        int expected;
    };

    std::vector<Testcase> tests;
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
        /* === Variable Section === */
        Testcase tc;
        int num_train, dim, k;
        if (!(iss >> num_train >> dim >> k)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tc.train.resize(num_train * dim);
        tc.labels.resize(num_train);
        tc.query_point.resize(dim);
        for (int i = 0; i < num_train; ++i) {
            for (int d = 0; d < dim; ++d) {
                if (!(iss >> tc.train[i * dim + d])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    continue;
                }
            }
            if (!(iss >> tc.labels[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int d = 0; d < dim; ++d) {
            if (!(iss >> tc.query_point[d])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const Testcase& tc = tests[idx];
        int result;

        result = i_result.read();

        bool passed = (result == tc.expected);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: train = [";
            for (size_t i = 0; i < tc.train.size(); ++i) {
                std::cout << tc.train[i] << " ";
            }
            std::cout << "], labels = [";
            for (size_t i = 0; i < tc.labels.size(); ++i) {
                std::cout << tc.labels[i] << " ";
            }
            std::cout << "], query_point = [";
            for (size_t i = 0; i < tc.query_point.size(); ++i) {
                std::cout << tc.query_point[i] << " ";
            }
            std::cout << "]\n";
            std::cout << "Output: " << result << "\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: train = [";
            for (size_t i = 0; i < tc.train.size(); ++i) {
                std::cerr << tc.train[i] << " ";
            }
            std::cerr << "], labels = [";
            for (size_t i = 0; i < tc.labels.size(); ++i) {
                std::cerr << tc.labels[i] << " ";
            }
            std::cerr << "], query_point = [";
            for (size_t i = 0; i < tc.query_point.size(); ++i) {
                std::cerr << tc.query_point[i] << " ";
            }
            std::cerr << "]\n";
            std::cerr << "Output: " << result << ", Expected: " << tc.expected << "\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully.\n";
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
