
#include <systemc.h>

SC_MODULE(ArrayMinMax) {
    sc_in_clk clk;          // Clock signal
    sc_out<int> min_out;    // Output for minimum element
    sc_out<int> max_out;    // Output for maximum element

    SC_CTOR(ArrayMinMax) {
        SC_METHOD(find_min_max);
        sensitive << clk.pos(); // Sensitivity to positive edge of clock
    }

    void find_min_max() {
        int arr[] = {12, 1234, 45, 67, 1}; // Array of integers
        int n = sizeof(arr) / sizeof(arr[0]); // Calculate number of elements

        // Initialize min and max with first element of array
        int min_val = arr[0];
        int max_val = arr[0];

        // Loop through the array to find min and max
        for (int i = 1; i < n; ++i) {
            if (arr[i] < min_val) min_val = arr[i];
            if (arr[i] > max_val) max_val = arr[i];
        }

        // Assign results to output ports
        min_out.write(min_val);
        max_out.write(max_val);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // Create a clock signal
    ArrayMinMax array_min_max("array_min_max"); // Instantiate the module

    // Bind clock signal to module
    array_min_max.clk(clk);

    // Run simulation for a few clock cycles
    sc_start(20, SC_NS);

    return 0;
}
