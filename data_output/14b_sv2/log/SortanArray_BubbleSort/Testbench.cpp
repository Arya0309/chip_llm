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
    struct Testcase { std::vector<int> arr; };
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
        int num;
        while (iss >> num) {
            tc.arr.push_back(num);
        }
        if (tc.arr.size() != N) {
            std::cerr << "Warning: Incorrect number of elements, skipping line: " << line << "\n";
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
        const auto& arr = tests[idx].arr;
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int num : arr) {
            o_arr.write(num);
        }
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase { std::vector<int> arr; };
    struct Golden { std::vector<int> expected; };

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
        int num;
        while (iss >> num) {
            tc.arr.push_back(num);
        }
        if (tc.arr.size() != N) {
            std::cerr << "Warning: Incorrect number of elements in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
    {
        std::ifstream fin("golden.txt");
        if (!fin.is_open()) {
            std::cerr << "Error: Unable to open golden.txt\n";
            sc_stop();
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            Golden g;
            int num;
            while (iss >> num) {
                g.expected.push_back(num);
            }
            if (g.expected.size() != N) {
                std::cerr << "Warning: Incorrect number of elements in golden.txt, skipping line: " << line << "\n";
                continue;
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        std::vector<int> result(N);

        for (int i = 0; i < N; ++i) {
            result[i] = i_sorted_arr.read();
        }

        bool passed = true;
        for (int i = 0; i < N; ++i) {
            if (result[i] != goldens[idx].expected[i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: ";
            for (int num : tests[idx].arr) {
                std::cout << num << " ";
            }
            std::cout << "\nOutput: ";
            for (int num : result) {
                std::cout << num << " ";
            }
            std::cout << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: ";
            for (int num : tests[idx].arr) {
                std::cerr << num << " ";
            }
            std::cerr << "\nOutput: ";
            for (int num : result) {
                std::cerr << num << " ";
            }
            std::cerr << "\nExpected: ";
            for (int num : goldens[idx].expected) {
                std::cerr << num << " ";
            }
            std::cerr << "\n\n";
            all_passed = false;
        }
    }
    /* === Variable Section End === */

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    }
    else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
