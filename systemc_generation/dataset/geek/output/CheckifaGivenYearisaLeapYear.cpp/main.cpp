
#include <systemc.h>

SC_MODULE(LeapYearChecker) {
    sc_in<int> year; // Input port for the year
    sc_out<bool> is_leap_year; // Output port indicating if the year is a leap year

    SC_CTOR(LeapYearChecker) {
        SC_METHOD(check_leap_year);
        sensitive << year;
    }

    void check_leap_year() {
        int y = year.read();
        bool result;

        if (y % 400 == 0) {
            result = true;
        } else if (y % 100 == 0) {
            result = false;
        } else if (y % 4 == 0) {
            result = true;
        } else {
            result = false;
        }

        is_leap_year.write(result);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> year;
    sc_signal<bool> is_leap_year;

    LeapYearChecker checker("checker");
    checker.year(year);
    checker.is_leap_year(is_leap_year);

    // Initialize the year
    year.write(2000);

    // Start simulation
    sc_start();

    // Output the result
    if (is_leap_year.read()) {
        cout << "Leap Year" << endl;
    } else {
        cout << "Not a Leap Year" << endl;
    }

    return 0;
}
