
#include <systemc.h>

// Module to perform Bubble Sort
SC_MODULE(BubbleSort) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> reset;        // Reset signal
    sc_vector<sc_inout<int>> nums; // Vector of inputs representing the array elements
    sc_out<bool> done;        // Output indicating sorting completion

    // Internal state variables
    int len;
    int i, j;
    bool isSwapped;
    bool running;

    // Constructor
    SC_CTOR(BubbleSort) {
        // Register the sorting process
        SC_METHOD(sort_process);
        sensitive << clk.posedge_event() << reset.posedge_event();
        dont_initialize();

        // Initialize internal variables
        len = nums.size();
        i = 0;
        j = 1;
        isSwapped = false;
        running = false;
    }

    // Sorting process
    void sort_process() {
        if (reset.read()) {
            // Reset the state variables
            i = 0;
            j = 1;
            isSwapped = false;
            running = false;
            done.write(false);
        } else if (!running) {
            // Start the sorting process
            i = 0;
            j = 1;
            isSwapped = false;
            running = true;
            done.write(false);
        } else {
            // Perform one step of bubble sort
            if (i < len) {
                if (j < len - i) {
                    if (nums[j].read() < nums[j - 1].read()) {
                        // Swap the elements
                        int temp = nums[j].read();
                        nums[j].write(nums[j - 1].read());
                        nums[j - 1].write(temp);
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        // If no elements were swapped, sorting is complete
                        running = false;
                        done.write(true);
                    } else {
                        // Move to the next iteration
                        i++;
                        j = 1;
                        isSwapped = false;
                    }
                }
            } else {
                // Sorting is complete
                running = false;
                done.write(true);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal
    sc_signal<bool> reset;      // Reset signal
    sc_vector<sc_signal<int>> nums; // Signals representing the array elements
    sc_signal<bool> done;       // Signal indicating sorting completion

    BubbleSort sorter;         // Instance of the BubbleSort module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), nums("nums", 5), sorter("sorter") {
        // Connect signals to module ports
        sorter.clk(clk);
        sorter.reset(reset);
        sorter.done(done);
        for (int i = 0; i < 5; i++) {
            sorter.nums[i](nums[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the array
        nums[0].write(1);
        nums[1].write(12);
        nums[2].write(6);
        nums[3].write(8);
        nums[4].write(10);

        // Apply reset
        reset.write(true);
        wait(1, SC_NS);
        reset.write(false);
        wait(1, SC_NS);

        // Wait until sorting is done
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print the sorted array
        cout << "After sorting the array is: \n";
        for (int i = 0; i < 5; i++) {
            cout << nums[i].read() << " ";
        }
        cout << "\n";

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
