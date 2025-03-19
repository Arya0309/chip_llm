
#include <systemc.h>

SC_MODULE(Adder) {
    SC_CTOR(Adder) {
        SC_METHOD(add_and_print);
        sensitive << start_event;
        dont_initialize();
    }

    void add_and_print() {
        int a = 11, b = 9;
        cout << "Sum: " << a + b << endl;
        sc_stop();  // Stop the simulation after printing the result
    }

    sc_event start_event;  // Event to trigger the method
};

int sc_main(int argc, char* argv[]) {
    Adder adder("adder");

    // Trigger the event to start the addition and print process
    adder.start_event.notify();

    // Start the simulation
    sc_start();

    return 0;
}
