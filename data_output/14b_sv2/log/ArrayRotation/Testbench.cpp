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
        int arr[6];
        int d;
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
        if (!(iss >> tc.d >> tc.arr[0] >> tc.arr[1] >> tc.arr[2] >> tc.arr[3] >> tc.arr[4] >> tc.arr[5])) {
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

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        TestCase& tc = tests[idx];
        o_d.write(tc.d);
        for (int i = 0; i < 6; ++i) {
            o_arr.write(tc.arr[i]);
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        int arr[6];
        int d;
    };
    struct Golden {
        int arr[6];
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
        if (!(iss >> tc.d >> tc.arr[0] >> tc.arr[1] >> tc.arr[2] >> tc.arr[3] >> tc.arr[4] >> tc.arr[5])) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
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
            if (!(iss >> g.arr[0] >> g.arr[1] >> g.arr[2] >> g.arr[3] >> g.arr[4] >> g.arr[5])) {
                std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
                continue;
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result[6];
        for (int i = 0; i < 6; ++i) {
            result[i] = i_result.read();
        }

        bool passed = true;
        for (int i = 0; i < 6; ++i) {
            if (result[i] != goldens[idx].arr[i]) {
                passed = false;
                break;
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: d = " << tests[idx].d << ", arr = [";
            for (int i = 0; i < 6; ++i) {
                std::cout << tests[idx].arr[i] << (i < 5 ? ", " : "]") << " ";
            }
            std::cout << "Output: [";
            for (int i = 0; i < 6; ++i) {
                std::cout << result[i] << (i < 5 ? ", " : "]") << " ";
            }
            std::cout << "\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: d = " << tests[idx].d << ", arr = [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << tests[idx].arr[i] << (i < 5 ? ", " : "]") << " ";
            }
            std::cerr << "Output: [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << result[i] << (i < 5 ? ", " : "]") << " ";
            }
            std::cerr << ", Expected: [";
            for (int i = 0; i < 6; ++i) {
                std::cerr << goldens[idx].arr[i] << (i < 5 ? ", " : "]") << " ";
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
