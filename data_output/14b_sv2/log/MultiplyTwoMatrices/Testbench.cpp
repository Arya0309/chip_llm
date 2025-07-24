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
        int mat1[2][2];
        int mat2[2][3];
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
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (!(iss >> tc.mat1[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (!(iss >> tc.mat2[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        tests.push_back(tc);
skip:
        continue;
    }
    fin.close();

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        TestCase& tc = tests[idx];
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                o_mat1.write(tc.mat1[i][j]);
            }
        }
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                o_mat2.write(tc.mat2[i][j]);
            }
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        int mat1[2][2];
        int mat2[2][3];
    };
    struct Golden {
        int result[2][3];
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
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (!(iss >> tc.mat1[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (!(iss >> tc.mat2[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        tests.push_back(tc);
skip:
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
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (!(iss >> g.result[i][j])) {
                        std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
                        goto skip;
                    }
                }
            }
            goldens.push_back(g);
skip:
            continue;
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        int result[2][3];
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                result[i][j] = i_result.read();
            }
        }

        bool passed = true;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (result[i][j] != goldens[idx].result[i][j]) {
                    passed = false;
                    break;
                }
            }
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input: mat1 = [";
            for (int i = 0; i < 2; ++i) {
                std::cout << "[";
                for (int j = 0; j < 2; ++j) {
                    std::cout << tests[idx].mat1[i][j];
                    if (j < 1) std::cout << ", ";
                }
                std::cout << "]";
                if (i < 1) std::cout << ", ";
            }
            std::cout << "], mat2 = [";
            for (int i = 0; i < 2; ++i) {
                std::cout << "[";
                for (int j = 0; j < 3; ++j) {
                    std::cout << tests[idx].mat2[i][j];
                    if (j < 2) std::cout << ", ";
                }
                std::cout << "]";
                if (i < 1) std::cout << ", ";
            }
            std::cout << "]\n";
            std::cout << "Output: [";
            for (int i = 0; i < 2; ++i) {
                std::cout << "[";
                for (int j = 0; j < 3; ++j) {
                    std::cout << result[i][j];
                    if (j < 2) std::cout << ", ";
                }
                std::cout << "]";
                if (i < 1) std::cout << ", ";
            }
            std::cout << "]\n\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input: mat1 = [";
            for (int i = 0; i < 2; ++i) {
                std::cerr << "[";
                for (int j = 0; j < 2; ++j) {
                    std::cerr << tests[idx].mat1[i][j];
                    if (j < 1) std::cerr << ", ";
                }
                std::cerr << "]";
                if (i < 1) std::cerr << ", ";
            }
            std::cerr << "], mat2 = [";
            for (int i = 0; i < 2; ++i) {
                std::cerr << "[";
                for (int j = 0; j < 3; ++j) {
                    std::cerr << tests[idx].mat2[i][j];
                    if (j < 2) std::cerr << ", ";
                }
                std::cerr << "]";
                if (i < 1) std::cerr << ", ";
            }
            std::cerr << "]\n";
            std::cerr << "Output: [";
            for (int i = 0; i < 2; ++i) {
                std::cerr << "[";
                for (int j = 0; j < 3; ++j) {
                    std::cerr << result[i][j];
                    if (j < 2) std::cerr << ", ";
                }
                std::cerr << "]";
                if (i < 1) std::cerr << ", ";
            }
            std::cerr << "]\nExpected: [";
            for (int i = 0; i < 2; ++i) {
                std::cerr << "[";
                for (int j = 0; j < 3; ++j) {
                    std::cerr << goldens[idx].result[i][j];
                    if (j < 2) std::cerr << ", ";
                }
                std::cerr << "]";
                if (i < 1) std::cerr << ", ";
            }
            std::cerr << "]\n\n";
            all_passed = false;
        }
    }

    if (all_passed) {
        std::cout << "All tests passed successfully.\" << std::endl;
    }
    else {
        SC_REPORT_FATAL("Testbench", "Some test cases failed.");
    }
    sc_stop();
}
