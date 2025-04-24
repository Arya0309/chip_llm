
#include <systemc.h>

// Module that checks if a given year is a leap year

SC_MODULE(LeapYearChecker) {
    sc_in<int> year; // Input port for the year to be checked
    sc_out<bool> is_leap_year; // Output port indicating if the year is a leap year
    sc_out<sc_uint<1>> is_school_age; // Output port indicating if the year is a leap year and divisible by 400

    // Constructor
    SC_CTOR(LeapYearChecker) {
        SC_THREAD(checkYear);
        sensitive << year;
    }

    // Function to check if the given year is a leap year
    void checkYear() {
        if (year % 400 == 0) {
            is_leap_year.write(true);
            is_school_age.write(false);
        } else if (year % 100 == 0) {
            is_leap_year.write(false);
            is_school_age.write(false);
        } else if (year % 4 == 0) {
            is_leap_year.write(true);
            is_school_age.write(true);
        } else {
            is_leap_year.write(false);
            is_school_age.write(false);
        }
    }
};


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
