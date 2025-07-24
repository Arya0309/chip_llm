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
    std::ifstream fin("input.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Unable to open input.txt\n";
        sc_stop();
        return;
    }

    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);

    unsigned char pixel;
    while (fin >> pixel) {
        o_input.write(pixel);
    }
    fin.close();
}

void Testbench::do_fetch() {
    std::ifstream fexp("expected_output.txt");
    if (!fexp.is_open()) {
        std::cerr << "Error: Unable to open expected_output.txt\n";
        sc_stop();
        return;
    }

    std::vector<unsigned char> expected_output;
    unsigned char exp_pixel;
    while (fexp >> exp_pixel) {
        expected_output.push_back(exp_pixel);
    }
    fexp.close();

    wait(1);

    bool all_passed = true;
    for (size_t idx = 0; idx < expected_output.size(); ++idx) {
        unsigned char result = i_output.read();
        bool passed = (result == expected_output[idx]);

        if (passed) {
            std::cout << "Test case " << idx + 1 << " passed.\n";
        } else {
            std::cerr << "Test case " << idx + 1 << " failed.\n";
            std::cerr << "Expected: " << static_cast<int>(expected_output[idx]) << ", Got: " << static_cast<int>(result) << "\n";
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
