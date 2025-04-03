
#include <systemc.h>

// Module to merge two sorted arrays
SC_MODULE(MergeArrays) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> rst_n;        // Reset signal (active low)
    sc_in<sc_uint<32>> n1;    // Size of the first array
    sc_in<sc_uint<32>> n2;    // Size of the second array
    sc_in<sc_uint<32>> arr1[100]; // First sorted array (fixed size for simplicity)
    sc_in<sc_uint<32>> arr2[100]; // Second sorted array (fixed size for simplicity)
    sc_out<sc_uint<32>> arr3[200]; // Merged and sorted array (fixed size for simplicity)

    // Internal variables
    sc_uint<32> i, j, k;

    // Constructor
    SC_CTOR(MergeArrays) {
        SC_METHOD(merge_arrays);
        sensitive << clk.posedge_event() << rst_n.negedge_event();
    }

    // Method to merge and sort arrays
    void merge_arrays() {
        if (!rst_n.read()) {
            // Reset state
            i = 0;
            j = 0;
            k = 0;
            for (int l = 0; l < 200; ++l) {
                arr3[l].write(0);
            }
        } else {
            // Merge arrays
            while (i < n1.read() && k < 200) {
                arr3[k++].write(arr1[i++]);
            }
            while (j < n2.read() && k < 200) {
                arr3[k++].write(arr2[j++]);
            }
            // Sort the merged array
            for (int l = 0; l < n1.read() + n2.read() - 1; ++l) {
                for (int m = 0; m < n1.read() + n2.read() - l - 1; ++m) {
                    if (arr3[m].read() > arr3[m+1].read()) {
                        sc_uint<32> temp = arr3[m].read();
                        arr3[m].write(arr3[m+1].read());
                        arr3[m+1].write(temp);
                    }
                }
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> rst_n;    // Reset signal (active low)
    sc_signal<sc_uint<32>> n1;// Size of the first array
    sc_signal<sc_uint<32>> n2;// Size of the second array
    sc_signal<sc_uint<32>> arr1[100]; // First sorted array
    sc_signal<sc_uint<32>> arr2[100]; // Second sorted array
    sc_signal<sc_uint<32>> arr3[200]; // Merged and sorted array

    MergeArrays merge_inst; // Instance of the MergeArrays module

    // Constructor
    SC_CTOR(Testbench) : merge_inst("merge_inst"), clk("clk", 10, SC_NS) {
        // Connect signals to ports
        merge_inst.clk(clk);
        merge_inst.rst_n(rst_n);
        merge_inst.n1(n1);
        merge_inst.n2(n2);
        for (int i = 0; i < 100; ++i) {
            merge_inst.arr1[i](arr1[i]);
            merge_inst.arr2[i](arr2[i]);
        }
        for (int i = 0; i < 200; ++i) {
            merge_inst.arr3[i](arr3[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize arrays
        n1.write(4);
        n2.write(4);
        arr1[0].write(1); arr1[1].write(3); arr1[2].write(5); arr1[3].write(7);
        arr2[0].write(2); arr2[1].write(4); arr2[2].write(6); arr2[3].write(8);

        // Apply reset
        rst_n.write(false);
        wait(1, SC_NS);
        rst_n.write(true);
        wait(10, SC_NS); // Wait for the merge and sort to complete

        // Print the merged and sorted array
        cout << "Array after merging:" << endl;
        for (int i = 0; i < 8; ++i) {
            cout << arr3[i].read() << " ";
        }
        cout << endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
