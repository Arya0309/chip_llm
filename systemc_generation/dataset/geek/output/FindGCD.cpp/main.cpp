
#include <systemc.h>

SC_MODULE(GCDModule) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> gcd_result;  // Output port for the GCD result

    SC_CTOR(GCDModule) {
        SC_METHOD(calculate_gcd);  // Register the method to calculate GCD
        sensitive << a << b;  // Sensitivity list: react to changes in 'a' or 'b'
    }

    void calculate_gcd() {
        int res = sc_min(a.read(), b.read());  // Find the minimum of a and b
        while (res > 1) {
            if (a.read() % res == 0 && b.read() % res == 0)
                break;
            res--;
        }
        gcd_result.write(res);  // Write the GCD result to the output port
    }
};

int sc_main(int argc, char* argv[]) {
    GCDModule gcd_module("gcd_module");  // Instantiate the GCD module

    sc_signal<int> a_signal, b_signal, gcd_result_signal;  // Declare signals

    // Connect ports to signals
    gcd_module.a(a_signal);
    gcd_module.b(b_signal);
    gcd_module.gcd_result(gcd_result_signal);

    // Initialize input signals
    a_signal = 12;
    b_signal = 16;

    // Start simulation
    sc_start();

    // Print the GCD result
    cout << "GCD of " << a_signal.read() << " and " << b_signal.read() << " is " << gcd_result_signal.read() << endl;

    return 0;
}
