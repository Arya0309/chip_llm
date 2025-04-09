
#include <systemc.h>

// Module that checks if a given year is a leap year

SC_MODULE(LeapYearChecker) {
    // Input port for the year
    sc_in<int> year;
    // Output port for the result (true if leap year, false otherwise)
    sc_out<bool> is_leap_year;

    // Constructor to register the process
    SC_CTOR(LeapYearChecker) {
        // Register the check_year method to be triggered by changes in the 'year' signal
        SC_METHOD(check_year);
        sensitive << year;
    }

    // Method to check if the year is a leap year
    void check_year() {
        int yr = year.read();  // Read the current year value from the input port
        bool result;

        // Leap year check logic
        if (yr % 400 == 0) {
            result = true;
        } else if (yr % 100 == 0) {
            result = false;
        } else if (yr % 4 == 0) {
            result = true;
        } else {
            result = false;
        }

        // Write the result to the output port
        is_leap_year.write(result);
    }
};

// Example usage of the LeapYearChecker module


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
