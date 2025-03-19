
#include <systemc.h>

SC_MODULE(SwapModule) {
    SC_CTOR(SwapModule) {
        SC_THREAD(swap_process);
    }

    void swap_process() {
        int a = 2, b = 3;

        // Print values before swapping
        cout << "Before swapping a = " << a << " , b = " << b << endl;

        // Temporary variable for swapping
        int temp;

        // Swapping algorithm
        temp = a;
        a = b;
        b = temp;

        // Print values after swapping
        cout << "After swapping a = " << a << " , b = " << b << endl;

        // End the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    SwapModule swap_module("SwapModule");
    sc_start(); // Start the simulation
    return 0;
}
