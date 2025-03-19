
#include <systemc.h>
#include <vector>
#include <iostream>

SC_MODULE(BinarySearchModule) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<sc_uint<32>> target;
    sc_out<bool> found;

    SC_CTOR(BinarySearchModule) {
        SC_METHOD(search);
        sensitive << clk.pos();
        dont_initialize();
    }

    void search() {
        if (rst.read()) {
            found.write(false);
            return;
        }

        std::vector<int> v = {1, 2, 3, 4, 5, 8, 9, 11}; // Sorted vector
        int low = 0;
        int high = v.size() - 1;
        bool result = false;

        while (low <= high) {
            int mid = ((high - low) / 2) + low;
            if (v[mid] == target.read()) {
                result = true;
                break;
            } else if (v[mid] > target.read()) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        found.write(result);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // Create a clock signal
    sc_signal<bool> rst("rst");
    sc_signal<sc_uint<32>> target("target");
    sc_signal<bool> found("found");

    BinarySearchModule bsm("bsm");
    bsm.clk(clk);
    bsm.rst(rst);
    bsm.target(target);
    bsm.found(found);

    sc_start(0, SC_NS);
    rst.write(true);
    sc_start(10, SC_NS);
    rst.write(false);
    target.write(8);
    sc_start(10, SC_NS);
    std::cout << "Target " << target.read() << (found.read() ? " found." : " NOT found.") << std::endl;

    return 0;
}
