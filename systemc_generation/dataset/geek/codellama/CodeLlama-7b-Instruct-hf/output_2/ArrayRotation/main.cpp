
#include <systemc.h>

// Module for rotating an array

SC_MODULE(Rotator) {
    sc_in<int> arr; // Input port for the array data
    sc_in<int> d; // Input port for the number of rotations
    sc_out<int> rotated_arr; // Output port for the rotated array

    // Constructor
    SC_CTOR(Rotator) {
        // Process to rotate the array
        SC_METHOD(rotate);
        sensitive << arr << d;
    }

    // Rotate the array
    void rotate() {
        // Temporary array to hold rotated data
        int temp[7];

        // Keeping track of the current index
        // of temp[]
        int k = 0;

        // Storing the n - d elements of
        // array arr[] to the front of temp[]
        for (int i = d; i < 7; i++) {
            temp[k] = arr[i];
            k++;
        }

        // Storing the first d elements of array arr[]
        // into temp
        for (int i = 0; i < d; i++) {
            temp[k] = arr[i];
            k++;
        }

        // Copying the elements of temp[] in arr[]
        // to get the final rotated array
        for (int i = 0; i < 7; i++) {
            arr[i] = temp[i];
        }

        // Output the rotated array
        rotated_arr.write(temp);
    }
};

// Module for printing the array

SC_MODULE(ArrayPrinter) {
    sc_in<int> arr; // Input port for the array data

    // Constructor
    SC_CTOR(ArrayPrinter) {
        // Process to print the array
        SC_METHOD(print_array);
        sensitive << arr;
    }

    // Print the array
    void print_array() {
        // Print the array elements
        for (int i = 0; i < 7; i++) {
            cout << arr[i] << " ";
        }
    }
};

// Driver code

SC_MODULE(Driver) {
    // Signals for the two modules
    sc_signal<int> arr;
    sc_signal<int> d;

    // Instantiate the modules
    Rotator rotator("rotator");
    ArrayPrinter printer("printer");

    // Connect the signals
    rotator.arr(arr);
    rotator.d(d);
    printer.arr(arr);

    // Run the simulation
    void run() {
        while (true) {
            rotator.rotate();
            printer.print_array();
        }
    }
};


SC_MODULE(Testbench) {
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<int> d;
    sc_signal<int> in_array[7];
    sc_signal<int> out_array[7];

    Rotator* rotator_inst;

    // Clock generator process
    void generate_clock() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }

    // Process to run tests
    void run_tests() {
        // Activate reset for one clock cycle
        rst.write(true);
        wait(10, SC_NS);
        rst.write(false);

        // Provide the input array and rotation offset (d = 2)
        int input[7] = { 1, 2, 3, 4, 5, 6, 7 };
        for (int i = 0; i < 7; i++) {
            in_array[i].write(input[i]);
        }
        d.write(2);

        // Wait for the rotator to process the new values.
        wait(10, SC_NS);

        // Expected output after rotation: {3, 4, 5, 6, 7, 1, 2}
        int expected[7] = { 3, 4, 5, 6, 7, 1, 2 };
        std::cout << "Rotated Array: ";
        for (int i = 0; i < 7; i++) {
            int out_val = out_array[i].read();
            // Assert that the output is as expected.
            assert(out_val == expected[i]);
            std::cout << out_val << " ";
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End simulation.
    }

    SC_CTOR(Testbench) {
        rotator_inst = new Rotator("Rotator");
        // Connect testbench signals to Rotator ports.
        rotator_inst->clk(clk);
        rotator_inst->rst(rst);
        rotator_inst->d(d);
        for (int i = 0; i < 7; i++) {
            rotator_inst->in_array[i](in_array[i]);
            rotator_inst->out_array[i](out_array[i]);
        }
        SC_THREAD(run_tests);
        SC_THREAD(generate_clock);
    }

    ~Testbench() {
        delete rotator_inst;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
