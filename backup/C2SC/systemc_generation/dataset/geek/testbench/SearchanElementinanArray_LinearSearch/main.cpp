#include <systemc.h>

// LinearSearch module: performs a linear search over an externally provided data array.
SC_MODULE(LinearSearch) {
    // Eight individual data input ports
    sc_in<int> data0, data1, data2, data3;
    sc_in<int> data4, data5, data6, data7;

    sc_in<int> key;        // Input port for the search key
    sc_out<int> index;     // Output port for the found index

    SC_CTOR(LinearSearch) {
        SC_METHOD(search);
        dont_initialize();
        sensitive << key
                  << data0 << data1 << data2 << data3
                  << data4 << data5 << data6 << data7;
    }

    void search() {
        int arr[8] = {
            data0.read(), data1.read(), data2.read(), data3.read(),
            data4.read(), data5.read(), data6.read(), data7.read()
        };

        int found_index = -1;
        int tgt = key.read();
        for (int i = 0; i < 8; ++i) {
            if (arr[i] == tgt) {
                found_index = i;
                break;
            }
        }
        index.write(found_index);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    // Signals for data array
    sc_signal<int> sig_data0, sig_data1, sig_data2, sig_data3;
    sc_signal<int> sig_data4, sig_data5, sig_data6, sig_data7;

    sc_signal<int> key_sig;    // key
    sc_signal<int> index_sig;  // result

    LinearSearch ls_inst;      // instance

    SC_CTOR(Testbench)
    : ls_inst("ls_inst")
    {
        // bind data signals
        ls_inst.data0(sig_data0);
        ls_inst.data1(sig_data1);
        ls_inst.data2(sig_data2);
        ls_inst.data3(sig_data3);
        ls_inst.data4(sig_data4);
        ls_inst.data5(sig_data5);
        ls_inst.data6(sig_data6);
        ls_inst.data7(sig_data7);

        ls_inst.key(key_sig);
        ls_inst.index(index_sig);

        // initialize array values
        int vals[8] = {1,2,3,4,5,8,9,11};
        sig_data0.write(vals[0]);
        sig_data1.write(vals[1]);
        sig_data2.write(vals[2]);
        sig_data3.write(vals[3]);
        sig_data4.write(vals[4]);
        sig_data5.write(vals[5]);
        sig_data6.write(vals[6]);
        sig_data7.write(vals[7]);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test 1: expect index 5
        key_sig.write(8);
        wait(1, SC_NS);
        assert(index_sig.read() == 5);
        std::cout << "Test 1 Passed: 8 found at index " << index_sig.read() << std::endl;

        // Test 2: expect -1
        key_sig.write(10);
        wait(1, SC_NS);
        assert(index_sig.read() == -1);
        std::cout << "Test 2 Passed: 10 not found (index: " << index_sig.read() << ")" << std::endl;

        // Test 3: expect 0
        key_sig.write(1);
        wait(1, SC_NS);
        assert(index_sig.read() == 0);
        std::cout << "Test 3 Passed: 1 found at index " << index_sig.read() << std::endl;

        std::cout << "All tests completed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
