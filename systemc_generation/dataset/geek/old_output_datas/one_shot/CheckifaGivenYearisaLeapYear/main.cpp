
#include <systemc.h>

// Module to check if a given year is a leap year
SC_MODULE(LeapYearChecker) {
    sc_in<int> year;          // Input port for the year
    sc_out<bool> is_leap_year; // Output port indicating if the year is a leap year

    // Constructor
    SC_CTOR(LeapYearChecker) {
        // Process to check leap year
        SC_METHOD(check_leap_year);
        sensitive << year;
    }

    // Method to check leap year
    void check_leap_year() {
        int y = year.read();
        if (y % 400 == 0) {
            is_leap_year.write(true);
        } else if (y % 100 == 0) {
            is_leap_year.write(false);
        } else if (y % 4 == 0) {
            is_leap_year.write(true);
        } else {
            is_leap_year.write(false);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> year;           // Signal for the year
    sc_signal<bool> is_leap_year;  // Signal indicating if the year is a leap year

    LeapYearChecker checker_inst; // Instance of the LeapYearChecker module

    // Constructor
    SC_CTOR(Testbench) : checker_inst("checker") {
        // Connect signals to ports
        checker_inst.year(year);
        checker_inst.is_leap_year(is_leap_year);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Year 2000
        year = 2000;
        wait(1, SC_NS); // Wait for the checker to process
        assert(is_leap_year.read() == true);
        cout << "Year 2000: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 2: Year 1900
        year = 1900;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 1900: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 3: Year 2004
        year = 2004;
        wait(1, SC_NS);
        assert(is_leap_year.read() == true);
        cout << "Year 2004: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 4: Year 2021
        year = 2021;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 2021: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

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
