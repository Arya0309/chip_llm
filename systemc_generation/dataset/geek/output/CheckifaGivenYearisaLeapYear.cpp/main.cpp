
#include <systemc.h>

// Module to check if a given year is a leap year
SC_MODULE(LeapYearChecker) {
    // Input port for the year
    sc_in<int> year_in;

    // Output port for the result
    sc_out<bool> is_leap_year_out;

    // Internal method to check leap year
    void checkLeapYear() {
        int year = year_in.read();
        bool is_leap_year = false;

        if (year % 400 == 0) {
            is_leap_year = true;
        } else if (year % 100 == 0) {
            is_leap_year = false;
        } else if (year % 4 == 0) {
            is_leap_year = true;
        } else {
            is_leap_year = false;
        }

        is_leap_year_out.write(is_leap_year);
    }

    // Constructor to register the process
    SC_CTOR(LeapYearChecker) {
        SC_METHOD(checkLeapYear);
        sensitive << year_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with LeapYearChecker
    sc_signal<int> year;
    sc_signal<bool> is_leap_year;

    // Instance of LeapYearChecker
    LeapYearChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the year
        year = 2000;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the result
        if (is_leap_year.read()) {
            cout << "Leap Year" << endl;
        } else {
            cout << "Not a Leap Year" << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.year_in(year);
        checker.is_leap_year_out(is_leap_year);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
