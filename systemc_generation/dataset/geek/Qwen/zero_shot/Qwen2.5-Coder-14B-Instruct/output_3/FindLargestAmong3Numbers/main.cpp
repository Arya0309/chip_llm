
#include <systemc.h>


SC_MODULE(LargestFinder) {
    sc_in<int> a;
    sc_in<int> b;
    sc_in<int> c;
    sc_out<int> largest;

    SC_CTOR(LargestFinder) {
        SC_METHOD(find_largest);
        sensitive << a << b << c;
    }

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


SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the first number
    sc_signal<int> b; // Signal for the second number
    sc_signal<int> c; // Signal for the third number
    sc_signal<int> largest; // Signal for the largest number

    LargestFinder largest_finder_inst; // Instance of the LargestFinder module

    // Constructor
    SC_CTOR(Testbench) : largest_finder_inst("largest_finder") {
        // Connect signals to ports
        largest_finder_inst.a(a);
        largest_finder_inst.b(b);
        largest_finder_inst.c(c);
        largest_finder_inst.largest(largest);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Basic test
        a = 1; b = 2; c = 11;
        wait(1, SC_NS); // Wait for the largest_finder to process
        assert(largest.read() == 11);
        cout << "Largest: " << largest.read() << endl;

        // Test case 2: All numbers are the same
        a = 5; b = 5; c = 5;
        wait(1, SC_NS);
        assert(largest.read() == 5);
        cout << "Largest: " << largest.read() << endl;

        // Test case 3: Two numbers are the same, one is larger
        a = 7; b = 7; c = 8;
        wait(1, SC_NS);
        assert(largest.read() == 8);
        cout << "Largest: " << largest.read() << endl;

        // Test case 4: Two numbers are the same, one is smaller
        a = 9; b = 9; c = 1;
        wait(1, SC_NS);
        assert(largest.read() == 9);
        cout << "Largest: " << largest.read() << endl;

        // Test case 5: All different numbers
        a = 3; b = 15; c = 9;
        wait(1, SC_NS);
        assert(largest.read() == 15);
        cout << "Largest: " << largest.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
