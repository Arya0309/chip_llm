#include <systemc.h>

SC_MODULE(BinarySearchModule) {
    // Eight individual input ports
    sc_in<int> v0, v1, v2, v3, v4, v5, v6, v7;
    sc_in<int> target;
    sc_out<bool> found;

    // Constructor
    SC_CTOR(BinarySearchModule) {
        SC_METHOD(binary_search);
        dont_initialize();
        sensitive << target
                  << v0 << v1 << v2 << v3
                  << v4 << v5 << v6 << v7;
    }

    void binary_search() {
        // Copy inputs into a local array for convenience
        int arr[8] = {
            v0.read(), v1.read(), v2.read(), v3.read(),
            v4.read(), v5.read(), v6.read(), v7.read()
        };

        int low = 0, high = 7;
        bool result = false;
        int tgt = target.read();

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid] == tgt) {
                result = true;
                break;
            } else if (arr[mid] > tgt) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        found.write(result);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> sig_v0, sig_v1, sig_v2, sig_v3;
    sc_signal<int> sig_v4, sig_v5, sig_v6, sig_v7;
    sc_signal<int> target_sig;
    sc_signal<bool> found_sig;

    BinarySearchModule bs_inst;

    SC_CTOR(Testbench)
    : bs_inst("bs_inst")
    {
        // Bind each port
        bs_inst.v0(sig_v0);
        bs_inst.v1(sig_v1);
        bs_inst.v2(sig_v2);
        bs_inst.v3(sig_v3);
        bs_inst.v4(sig_v4);
        bs_inst.v5(sig_v5);
        bs_inst.v6(sig_v6);
        bs_inst.v7(sig_v7);
        bs_inst.target(target_sig);
        bs_inst.found(found_sig);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Initialize sorted vector
        int vals[8] = {1,2,3,4,5,8,9,11};
        sig_v0.write(vals[0]);
        sig_v1.write(vals[1]);
        sig_v2.write(vals[2]);
        sig_v3.write(vals[3]);
        sig_v4.write(vals[4]);
        sig_v5.write(vals[5]);
        sig_v6.write(vals[6]);
        sig_v7.write(vals[7]);

        // Test 1: found
        target_sig.write(8);
        wait(1, SC_NS);
        assert(found_sig.read() == true);
        std::cout << "Found 8: " << found_sig.read() << std::endl;

        // Test 2: not found
        target_sig.write(7);
        wait(1, SC_NS);
        assert(found_sig.read() == false);
        std::cout << "Found 7: " << found_sig.read() << std::endl;

        std::cout << "All tests completed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
