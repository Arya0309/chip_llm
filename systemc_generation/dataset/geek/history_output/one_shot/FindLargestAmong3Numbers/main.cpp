
#include <systemc.h>

// Module to find the largest among three numbers
SC_MODULE(FindLargest) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_in<int> c;  // Input port for the third number
    sc_out<int> largest; // Output port for the largest number

    // Constructor
    SC_CTOR(FindLargest) {
        // Process to determine the largest number
        SC_METHOD(find_largest);
        sensitive << a << b << c;
    }

    // Method to find the largest number
    void find_largest() {
        int max_val;
        if (a.read() >= b.read()) {
            if (a.read() >= c.read())
                max_val = a.read();
            else
                max_val = c.read();
        } else {
            if (b.read() >= c.read())
                max_val = b.read();
            else
                max_val = c.read();
        }
        largest.write(max_val);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the first number
    sc_signal<int> b; // Signal for the second number
    sc_signal<int> c; // Signal for the third number
    sc_signal<int> largest; // Signal for the largest number

    FindLargest find_largest_inst; // Instance of the FindLargest module

    // Constructor
    SC_CTOR(Testbench) : find_largest_inst("find_largest") {
        // Connect signals to ports
        find_largest_inst.a(a);
        find_largest_inst.b(b);
        find_largest_inst.c(c);
        find_largest_inst.largest(largest);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize inputs
        a = 1; b = 2; c = 11;
        wait(1, SC_NS); // Wait for the module to process
        cout << "The largest number is: " << largest.read() << endl;

        // Additional test case
        a = 10; b = 20; c = 15;
        wait(1, SC_NS);
        cout << "The largest number is: " << largest.read() << endl;

        // Additional test case
        a = 30; b = 25; c = 25;
        wait(1, SC_NS);
        cout << "The largest number is: " << largest.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
