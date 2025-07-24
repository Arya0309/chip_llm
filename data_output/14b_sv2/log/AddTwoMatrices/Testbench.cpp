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
        int A[N][N];
        int B[N][N];
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.A[i][j])) {
                    std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.B[i][j])) {
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                o_a.write(tc.A[i][j]);
                o_b.write(tc.B[i][j]);
            }
        }
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        int A[N][N];
        int B[N][N];
    };
    struct Golden {
        int C[N][N];
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
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.A[i][j])) {
                    std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                    goto skip;
                }
            }
        }
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!(iss >> tc.B[i][j])) {
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
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    if (!(iss >> g.C[i][j])) {
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
        int C[N][N];
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                C[i][j] = i_result.read();
            }
        }

        bool passed = true;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (C[i][j] != goldens[idx].C[i][j]) {
                    passed = false;
                    break;
                }
            }
            if (!passed) break;
        }

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
            std::cout << "Input A:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cout << tests[idx].A[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Input B:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cout << tests[idx].B[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Output:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cout << C[i][j] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Input A:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cerr << tests[idx].A[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Input B:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cerr << tests[idx].B[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Output:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cerr << C[i][j] << " ";
                }
                std::cerr << "\n";
            }
            std::cerr << "Expected:\n";
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    std::cerr << goldens[idx].C[i][j] << " ";
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
