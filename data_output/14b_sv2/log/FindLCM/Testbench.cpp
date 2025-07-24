#include "Testbench.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

SC_MODULE(Testbench) {
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
    sc_fifo_out<int> o_a;
    sc_fifo_out<int> o_b;
    sc_fifo_in<int> i_result;

    SC_CTOR(Testbench) {
        SC_THREAD(feed_data);
        sensitive << i_clk.pos();
        dont_initialize();
        SC_THREAD(check_results);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void feed_data() {
        ifstream fin("input.txt");
        if (!fin.is_open()) {
            cerr << "Error: Could not open input.txt" << endl;
            sc_stop();
            return;
        }

        o_rst.write(false);
        wait(5);
        o_rst.write(true);
        wait(1);

        int a, b;
        while (fin >> a >> b) {
            o_a.write(a);
            o_b.write(b);
            wait();
        }

        fin.close();
        sc_stop();
    }

    void check_results() {
        ifstream fin("expected_output.txt");
        if (!fin.is_open()) {
            cerr << "Error: Could not open expected_output.txt" << endl;
            sc_stop();
            return;
        }

        int expected_lcm;
        while (fin >> expected_lcm) {
            int actual_lcm = i_result.read();
            if (actual_lcm != expected_lcm) {
                cerr << "Mismatch: Expected LCM = " << expected_lcm << ", but got LCM = " << actual_lcm << endl;
                sc_stop();
            }
            wait();
        }

        fin.close();
        cout << "All test cases passed successfully." << endl;
        sc_stop();
    }
};
