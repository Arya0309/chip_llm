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
        float input[INPUT_SIZE];
        float w1[HIDDEN_SIZE][INPUT_SIZE];
        float b1[HIDDEN_SIZE];
        float w2[OUTPUT_SIZE][HIDDEN_SIZE];
        float b2[OUTPUT_SIZE];
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
        for (int i = 0; i < INPUT_SIZE; ++i) {
            if (!(iss >> tc.input[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                if (!(iss >> tc.w1[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    continue;
                }
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            if (!(iss >> tc.b1[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                if (!(iss >> tc.w2[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    continue;
                }
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            if (!(iss >> tc.b2[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        tests.push_back(tc);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        TestCase& tc = tests[idx];
        for (int i = 0; i < INPUT_SIZE; ++i) {
            o_input.write(tc.input[i]);
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                o_w1.write(tc.w1[i][j]);
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            o_b1.write(tc.b1[i]);
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                o_w2.write(tc.w2[i][j]);
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            o_b2.write(tc.b2[i]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        float input[INPUT_SIZE];
        float w1[HIDDEN_SIZE][INPUT_SIZE];
        float b1[HIDDEN_SIZE];
        float w2[OUTPUT_SIZE][HIDDEN_SIZE];
        float b2[OUTPUT_SIZE];
    };
    struct Golden {
        float expected[OUTPUT_SIZE];
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
        for (int i = 0; i < INPUT_SIZE; ++i) {
            if (!(iss >> tc.input[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            for (int j = 0; j < INPUT_SIZE; ++j) {
                if (!(iss >> tc.w1[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    continue;
                }
            }
        }
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            if (!(iss >> tc.b1[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            for (int j = 0; j < HIDDEN_SIZE; ++j) {
                if (!(iss >> tc.w2[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    continue;
                }
            }
        }
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            if (!(iss >> tc.b2[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        tests.push_back(tc);
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
            for (int i = 0; i < OUTPUT_SIZE; ++i) {
                if (!(iss >> g.expected[i])) {
                    std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
                    continue;
                }
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        float output[OUTPUT_SIZE];
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            output[i] = i_output.read();
        }
        bool passed = true;
        for (int i = 0; i < OUTPUT_SIZE; ++i) {
            if (std::abs(output[i] - goldens[idx].expected[i]) > 1e-6) {
                passed = false;
                break;
            }
        }
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
