c
// SystemC code to check if a given year is a leap year or not
#include <systemc.h>

// Module that checks if a given year is a leap year or not

SC_MODULE(LeapYearChecker) {
    // Input port for the year
    sc_in<int> year;
    // Output port for the leap year result
    sc_out<bool> is_leap_year;

    // Constructor
    SC_CTOR(LeapYearChecker) {
        // Process to determine if the year is a leap year
        SC_METHOD(CheckYear);
        sensitive << year;
    }

    // Function to determine if the given year is a leap year or not
    void CheckYear() {
        int year = year.read(); // Read the input year

        if (year % 400 == 0) { // If the year is a multiple of 400, it is a leap year
            is_leap_year.write(true);
        } else if (year % 100 == 0 && year % 4 != 0) { // If the year is not a multiple of 100 and a multiple of 4, it is not a leap year
            is_leap_year.write(false);
        } else if (year % 4 == 0) { // If the year is a multiple of 4, but not a multiple of 100, it is a leap year
            is_leap_year.write(true);
        } else { // All other years are not leap years
            is_leap_year.write(false);
        }
    }
};

// Testbench for the LeapYearChecker module

SC_MODULE(LeapYearCheckerTestbench) {
    // Input signals for the LeapYearChecker module
    sc_in<int> year;
    // Output signals for the LeapYearChecker module
    sc_out<bool> is_leap_year;

    // Instantiate the LeapYearChecker module
    LeapYearChecker leap_year_module("leap_year_module");

    // Connect the input and output signals
    leap_year_module.year(year);
    leap_year_module.is_leap_year(is_leap_year);

    // Run the testbench
    sc_start();

    // Check the output of the LeapYearChecker module
    if (is_leap_year.read() == true) {
        cout << "Year " << year.read() << " is a leap year." << endl;
    } else {
        cout << "Year " << year.read() << " is not a leap year." << endl;
    }
}

// Main function to run the LeapYearCheckerTestbench
int main() {
    LeapYearCheckerTestbench testbench("testbench");
    testbench.year(2000); // Set the input year to 2000
    testbench.run(); // Run the testbench
    return 0;
}


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
