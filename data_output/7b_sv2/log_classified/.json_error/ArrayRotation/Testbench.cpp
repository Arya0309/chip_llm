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
    struct Testcase { int arr[6], d; };
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
        for (int i = 0; i < 6; ++i) {
            if (!(iss >> tc.arr[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.d)) {
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
        int arr[6];
        int d = tests[idx].d;
        for (int i = 0; i < 6; ++i) {
            arr[i] = tests[idx].arr[i];
        }
        /* === Variable Section End === */

        /* === Variable Section === */
        for (int i = 0; i < 6; ++i) {
            o_arr.write(arr[i]);
        }
        o_d.write(d);
        /* === Variable Section End === */

        wait();
    }
}


void Testbench::do_fetch() {
    struct Testcase  { int arr[6], d; };
    struct Golden { int arr[6]; };

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
        for (int i = 0; i < 6; ++i) {
            if (!(iss >> tc.arr[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.d)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
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
            for (int i = 0; i < 6; ++i) {
                if (!(iss >> g.arr[i])) {
                    std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                    continue;
                }
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    /* === Variable Section === */
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int arr[6];

        for (int i = 0; i < 6; ++i) {
            arr[i] = i_result.read();
        }

        bool passed = true;
        for (int i = 0; i < 6; ++i) {
            if (arr[i] != goldens[idx].arr[i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: d = " << tests[idx].d << ", arr = [";
            for (int i = 0; i < 6; ++i) {
                std::cout << tests[idx].arr[i] << " ";
            }
            std::cout << "]\n";
            std::cout << "Output: [";
            for (int i = 0; i < 6; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << "]\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: d = " << tests[idx].d << ", arr = [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << tests[idx].arr[i] << " ";
            }
            std::cerr << "]\n";
            std::cerr << "Output: [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << arr[i] << " ";
            }
            std::cerr << "], Expected: [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << goldens[idx].arr[i] << " ";
            }
            std::cerr << "]\n\n";
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
