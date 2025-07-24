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
        int array[6];
        int target;
        bool expected;
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
        for (int i = 0; i < 6; ++i) {
            if (!(iss >> tc.array[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.target >> tc.expected)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        tests.push_back(tc);
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (const auto& test : tests) {
        for (int i = 0; i < 6; ++i) {
            o_array.write(test.array[i]);
        }
        o_target.write(test.target);
        wait();
    }
}

void Testbench::do_fetch() {
    std::vector<bool> results;
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
        for (int i = 0; i < 6; ++i) {
            if (!(iss >> tc.array[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                continue;
            }
        }
        if (!(iss >> tc.target >> tc.expected)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        results.push_back(tc.expected);
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < results.size(); ++idx) {
        bool result = i_result.read();
        bool passed = (result == results[idx]);
        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: array = [";
            for (int i = 0; i < 6; ++i) {
                std::cout << i_array.read();
                if (i < 5) std::cout << ", ";
            }
            std::cout << "], target = " << i_target.read() << "\n";
            std::cout << "Output: " << result << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: array = [";
            for (int i = 0; i < 6; ++i) {
                std::cout << i_array.read();
                if (i < 5) std::cout << ", ";
            }
            std::cerr << "], target = " << i_target.read() << "\n";
            std::cerr << "Output: " << result << ", Expected: " << results[idx] << "\n\n";
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