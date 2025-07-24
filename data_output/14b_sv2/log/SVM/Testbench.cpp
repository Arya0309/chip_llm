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
        double X[MAX_N * MAX_D];
        int y[MAX_N];
        int n;
        int d;
        double C;
        double lr;
        int epochs;
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
        if (!(iss >> tc.n >> tc.d >> tc.C >> tc.lr >> tc.epochs)) {
            std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
            continue;
        }
        for (int i = 0; i < tc.n * tc.d; ++i) {
            if (!(iss >> tc.X[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                break;
            }
        }
        for (int i = 0; i < tc.n; ++i) {
            if (!(iss >> tc.y[i])) {
                std::cerr << "Warning: Incorrect format, skipping line: " << line << "\n";
                break;
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
        for (int i = 0; i < tc.n * tc.d; ++i) {
            o_X.write(tc.X[i]);
        }
        for (int i = 0; i < tc.n; ++i) {
            o_y.write(tc.y[i]);
        }
        o_n.write(tc.n);
        o_d.write(tc.d);
        o_C.write(tc.C);
        o_lr.write(tc.lr);
        o_epochs.write(tc.epochs);
        wait();
    }
}

void Testbench::do_fetch() {
    struct TestCase {
        double X[MAX_N * MAX_D];
        int y[MAX_N];
        int n;
        int d;
        double C;
        double lr;
        int epochs;
    };
    struct Golden {
        double w[MAX_D];
        double b;
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
        if (!(iss >> tc.n >> tc.d >> tc.C >> tc.lr >> tc.epochs)) {
            std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
            continue;
        }
        for (int i = 0; i < tc.n * tc.d; ++i) {
            if (!(iss >> tc.X[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                break;
            }
        }
        for (int i = 0; i < tc.n; ++i) {
            if (!(iss >> tc.y[i])) {
                std::cerr << "Warning: Incorrect format in testcases.txt, skipping line: " << line << "\n";
                break;
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
            for (int j = 0; j < MAX_D; ++j) {
                if (!(iss >> g.w[j])) {
                    std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
                    break;
                }
            }
            if (!(iss >> g.b)) {
                std::cerr << "Warning: Incorrect format in golden.txt, skipping line: " << line << "\n";
                continue;
            }
            goldens.push_back(g);
        }
    }
    fin.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < goldens.size(); ++idx) {
        double w[MAX_D];
        double b;
        for (int j = 0; j < MAX_D; ++j) {
            w[j] = i_w.read();
        }
        b = i_b.read();

        bool passed = true;
        for (int j = 0; j < MAX_D; ++j) {
            if (fabs(w[j] - goldens[idx].w[j]) > 1e-6) {
                passed = false;
                break;
            }
        }
        if (fabs(b - goldens[idx].b) > 1e-6) {
            passed = false;
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
