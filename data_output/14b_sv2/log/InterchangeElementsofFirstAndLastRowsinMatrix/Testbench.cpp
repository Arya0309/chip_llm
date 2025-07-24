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
    struct Testcase { int matrix[ROWS][COLS]; };
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
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (!(iss >> tc.matrix[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
        }
        tests.push_back(tc);
skip_testcase:
        /* === Variable Section End === */
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        /* === Variable Section === */
        int matrix[ROWS][COLS];
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                matrix[i][j] = tests[idx].matrix[i][j];
            }
        }
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                o_matrix.write(matrix[i][j]);
            }
        }
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase { int matrix[ROWS][COLS]; };
    struct Golden { int matrix[ROWS][COLS]; };

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
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (!(iss >> tc.matrix[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
        }
        tests.push_back(tc);
skip_testcase:
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
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    if (!(iss >> g.matrix[i][j])) {
                        std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                        goto skip_golden;
                    }
                }
            }
            goldens.push_back(g);
skip_golden:
            /* === Variable Section End === */
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result_matrix[ROWS][COLS];
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                result_matrix[i][j] = i_result.read();
            }
        }

        bool passed = true;
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (result_matrix[i][j] != goldens[idx].matrix[i][j]) {
                    passed = false;
                    break;
                }
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input:\n";
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    std::cout << tests[idx].matrix[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Output:\n";
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    std::cout << result_matrix[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input:\n";
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    std::cerr << tests[idx].matrix[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Output:\n";
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    std::cerr << result_matrix[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Expected:\n";
            for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLS; ++j) {
                    std::cerr << goldens[idx].matrix[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "\n";
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
