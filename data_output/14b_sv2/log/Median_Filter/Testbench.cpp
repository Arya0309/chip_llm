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
        int input[ROWS][COLS];
        int window_size;
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
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (!(iss >> tc.input[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
        }
        if (!(iss >> tc.window_size)) {
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
        o_window_size.write(tc.window_size);
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                o_input.write(tc.input[i][j]);
            }
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        int input[ROWS][COLS];
        int window_size;
    };
    struct Golden {
        int output[ROWS][COLS];
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
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (!(iss >> tc.input[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
        }
        if (!(iss >> tc.window_size)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            goto skip_testcase;
        }
        tests.push_back(tc);
skip_testcase:
        continue;
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
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    if (!(iss >> g.output[i][j])) {
                        std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
                        goto skip_golden;
                    }
                }
            }
            goldens.push_back(g);
skip_golden:
            continue;
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int output[ROWS][COLS];
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                output[i][j] = i_output.read();
            }
        }

        bool passed = true;
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (output[i][j] != goldens[idx].output[i][j]) {
                    passed = false;
                    break;
                }
            }
            if (!passed) break;
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully." << std::endl;
    } else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
