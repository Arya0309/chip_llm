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
        double input[IN_H * IN_W];
        double kernel[KER_H * KER_W];
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
        for (int i = 0; i < IN_H * IN_W; ++i) {
            if (!(iss >> tc.input[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < KER_H * KER_W; ++i) {
            if (!(iss >> tc.kernel[i])) {
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
        for (int i = 0; i < IN_H * IN_W; ++i) {
            o_input.write(tc.input[i]);
        }
        for (int i = 0; i < KER_H * KER_W; ++i) {
            o_kernel.write(tc.kernel[i]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        double input[IN_H * IN_W];
        double kernel[KER_H * KER_W];
    };
    struct Golden {
        double expected[IN_H * IN_W];
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
        for (int i = 0; i < IN_H * IN_W; ++i) {
            if (!(iss >> tc.input[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        for (int i = 0; i < KER_H * KER_W; ++i) {
            if (!(iss >> tc.kernel[i])) {
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
            for (int i = 0; i < IN_H * IN_W; ++i) {
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
        double output[IN_H * IN_W];
        for (int i = 0; i < IN_H * IN_W; ++i) {
            output[i] = i_output.read();
        }

        bool passed = true;
        for (int i = 0; i < IN_H * IN_W; ++i) {
            if (output[i] != goldens[idx].expected[i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: ";
            for (int i = 0; i < IN_H * IN_W; ++i) {
                std::cout << tests[idx].input[i] << " ";
            }
            std::cout << "\nKernel: ";
            for (int i = 0; i < KER_H * KER_W; ++i) {
                std::cout << tests[idx].kernel[i] << " ";
            }
            std::cout << "\nOutput: ";
            for (int i = 0; i < IN_H * IN_W; ++i) {
                std::cout << output[i] << " ";
            }
            std::cout << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: ";
            for (int i = 0; i < IN_H * IN_W; ++i) {
                std::cerr << tests[idx].input[i] << " ";
            }
            std::cerr << "\nKernel: ";
            for (int i = 0; i < KER_H * KER_W; ++i) {
                std::cerr << tests[idx].kernel[i] << " ";
            }
            std::cerr << "\nOutput: ";
            for (int i = 0; i < IN_H * IN_W; ++i) {
                std::cerr << output[i] << " ";
            }
            std::cerr << "\nExpected: ";
            for (int i = 0; i < IN_H * IN_W; ++i) {
                std::cerr << goldens[idx].expected[i] << " ";
            }
            std::cerr << "\n\n";
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
