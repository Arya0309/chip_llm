
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

    // Method to determine if the year is a leap year
    void check_leap_year() {
        int year_val = year.read();
        bool leap = false;

        if (year_val % 400 == 0) {
            leap = true;
        } else if (year_val % 100 == 0) {
            leap = false;
        } else if (year_val % 4 == 0) {
            leap = true;
        } else {
            leap = false;
        }

        is_leap_year.write(leap);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> year;             // Signal for the year
    sc_signal<bool> is_leap_year;    // Signal for the leap year check result

    LeapYearChecker checker_inst;    // Instance of the LeapYearChecker module

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
        // Test case 1: Year 2000 (Leap Year)
        year = 2000;
        wait(1, SC_NS);
        assert(is_leap_year.read() == true);
        cout << "Year 2000: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 2: Year 1900 (Not a Leap Year)
        year = 1900;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 1900: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 3: Year 2004 (Leap Year)
        year = 2004;
        wait(1, SC_NS);
        assert(is_leap_year.read() == true);
        cout << "Year 2004: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 4: Year 2001 (Not a Leap Year)
        year = 2001;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 2001: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

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
