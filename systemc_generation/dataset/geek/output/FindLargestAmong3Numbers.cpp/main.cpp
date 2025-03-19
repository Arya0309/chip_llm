
#include <systemc.h>

SC_MODULE(LargestNumberFinder) {
    SC_CTOR(LargestNumberFinder) {
        SC_METHOD(find_largest);
        sensitive << start_signal;
    }

    void find_largest() {
        int a = 1, b = 2, c = 11;
        int largest;

        if (a >= b) {
            if (a >= c)
                largest = a;
            else
                largest = c;
        } else {
            if (b >= c)
                largest = b;
            else
                largest = c;
        }

        cout << "The largest number is: " << largest << endl;
    }

    sc_in<bool> start_signal; // Signal to trigger the method
};

int sc_main(int argc, char* argv[]) {
    LargestNumberFinder finder("finder");
    sc_signal<bool> start;

    finder.start_signal(start);

    start.write(true); // Trigger the method

    sc_start(); // Start the simulation

    return 0;
}
