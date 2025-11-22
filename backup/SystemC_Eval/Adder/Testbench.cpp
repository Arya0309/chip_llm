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
    struct Testcase { int a, b; };
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
        if (!(iss >> tc.a >> tc.b)) {
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

    for (const auto &tc : tests) {
        /* === Variable Section === */
        o_a.write(tc.a);
        o_b.write(tc.b);
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase  { int a, b; };
    struct Golden { int expected; };

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
        if (!(iss >> tc.a >> tc.b)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
        /* === Variable Section End === */
    }
    fin.close();

    std::vector<Golden> goldens;
    std::ifstream gin("golden.txt");
    if (!gin.is_open()) {
        std::cerr << "Error: Unable to open golden.txt\n";
        sc_stop();
        return;
    }

    while (std::getline(gin, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        /* === Variable Section === */
        Golden g;
        if (!(iss >> g.expected)) {
            std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
            continue;
            return;
        }
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            Golden g;
            if (!(iss >> g.expected)) {
                std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                continue;
            }
            goldens.push_back(g);
        }
    }
    gin.close();

    wait(1);

    size_t passed_count = 0;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result;
        result = i_result.read();

        bool passed = (result == goldens[idx].expected);
        if (passed) ++passed_count;
        // if (passed) {
        //     ++passed_count;
        //     std::cout << "Test case " << idx + 1 << " passed.\n";
        // } else {
        //     std::cerr << "Test case " << idx + 1 << " failed.\n";
        //     std::cerr << "Input: a = " << tests[idx].a << ", b = " << tests[idx].b << "\n";
        //     std::cerr << "Output: " << result << "\n";
        //     std::cerr << "Expected: " << goldens[idx].expected << "\n\n";
        // }
    }
    /* === Variable Section End === */

    if (passed_count == goldens.size()) {
        std::cout << "All test cases passed!\n";
    } else {
        std::cerr << goldens.size() - passed_count << " out of " << goldens.size() << " test cases failed.\n";
    }
    sc_stop();
}
