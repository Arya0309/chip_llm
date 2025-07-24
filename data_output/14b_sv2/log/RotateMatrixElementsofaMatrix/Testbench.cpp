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
        int mat[4][4];
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
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (!(iss >> tc.mat[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
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
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                o_mat.write(tc.mat[i][j]);
            }
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        int mat[4][4];
    };
    struct Golden {
        int mat[4][4];
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
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (!(iss >> tc.mat[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip_testcase;
                }
            }
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
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (!(iss >> g.mat[i][j])) {
                        std::cerr << "Warning: Incorrect format in golden.txt, skip: " << line << "\n";
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
        TestCase& tc = tests[idx];
        Golden& g = goldens[idx];
        int result[4][4];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = i_result.read();
            }
        }

        bool passed = true;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (result[i][j] != g.mat[i][j]) {
                    passed = false;
                    break;
                }
            }
            if (!passed) break;
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input:\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cout << tc.mat[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Output:\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cout << result[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input:\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cerr << tc.mat[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Output:\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cerr << result[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Expected:\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cerr << g.mat[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "\n";
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
