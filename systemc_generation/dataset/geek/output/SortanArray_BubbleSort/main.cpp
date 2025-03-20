
#include <systemc.h>

// Module to perform Bubble Sort
SC_MODULE(BubbleSortModule) {
    sc_in<bool> clk;             // Clock signal
    sc_in<bool> rst;             // Reset signal
    sc_in<sc_uint<8>> arr_size;  // Size of the array
    sc_inout<sc_uint<8>> arr[4]; // Array to be sorted (fixed size for simplicity)
    sc_out<bool> done;            // Done signal indicating sorting completion

    // Internal variables
    int i, j;
    bool sorting;
    sc_uint<8> temp;

    // Constructor
    SC_CTOR(BubbleSortModule) {
        // Register the process for sorting
        SC_METHOD(sort_process);
        sensitive << clk.pos();
        dont_initialize();

        // Register the reset process
        SC_METHOD(reset_process);
        sensitive << rst;
    }

    // Process to handle reset
    void reset_process() {
        if (rst.read()) {
            i = 0;
            j = 0;
            sorting = false;
            done.write(false);
        }
    }

    // Process to perform Bubble Sort
    void sort_process() {
        if (!sorting) {
            if (i < arr_size.read() - 1) {
                sorting = true;
                j = 0;
            } else {
                done.write(true);
            }
        }

        if (sorting) {
            if (j < arr_size.read() - i - 1) {
                if (arr[j].read() > arr[j + 1].read()) {
                    temp = arr[j].read();
                    arr[j].write(arr[j + 1].read());
                    arr[j + 1].write(temp);
                }
                j++;
            } else {
                i++;
                sorting = false;
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;                 // Clock signal
    sc_signal<bool> rst;          // Reset signal
    sc_signal<sc_uint<8>> arr_size;// Size of the array
    sc_signal<sc_uint<8>> arr[4];  // Array to be sorted (fixed size for simplicity)
    sc_signal<bool> done;          // Done signal indicating sorting completion

    BubbleSortModule bubble_sort_inst; // Instance of the BubbleSortModule

    // Constructor
    SC_CTOR(Testbench)
    : bubble_sort_inst("bubble_sort_inst"), clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true) {
        // Connect signals to ports
        bubble_sort_inst.clk(clk);
        bubble_sort_inst.rst(rst);
        bubble_sort_inst.arr_size(arr_size);
        for (int k = 0; k < 4; k++) {
            bubble_sort_inst.arr[k](arr[k]);
        }
        bubble_sort_inst.done(done);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array
        arr[0].write(6);
        arr[1].write(0);
        arr[2].write(3);
        arr[3].write(5);
        arr_size.write(4);

        // Assert reset
        rst.write(true);
        wait(1, SC_NS);
        rst.write(false);
        wait(1, SC_NS);

        // Wait for sorting to complete
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print sorted array
        for (int k = 0; k < 4; k++) {
            std::cout << arr[k].read() << " ";
        }
        std::cout << std::endl;

        // Stop simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
