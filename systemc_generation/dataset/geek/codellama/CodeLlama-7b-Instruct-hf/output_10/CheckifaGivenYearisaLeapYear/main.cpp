scss
#include <systemc.h>

// Module that checks if a given year is a leap year or not

SC_MODULE(LeapYearChecker) {
    sc_in<int> year_input;  // Input port for the year
    sc_out<bool> leap_year_output; // Output port for the leap year status

    // Constructor
    SC_CTOR(LeapYearChecker) {
        SC_THREAD(process);
        sensitive << year_input << leap_year_output;
    }

    // Function to check whether the input year is a leap year
    bool checkYear(int year) {
        if (year % 400 == 0) {
            return true;
        }

        // not a leap year if divisible by 100
        // but not divisible by 400
        else if (year % 100 == 0 && year % 400 != 0) {
            return false;
        }

        // leap year if not divisible by 100
        // but divisible by 4
        else if (year % 4 == 0 && year % 100 != 0) {
            return true;
        }

        // all other years are not leap years
        else {
            return false;
        }
    }

    // Process method that checks the leap year and sets the output
    void process() {
        while (true) {
            wait();

            bool leap_year = checkYear(year_input.read());
            leap_year_output.write(leap_year);
        }
    }
};

// Testbench code that drives the LeapYearChecker


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
