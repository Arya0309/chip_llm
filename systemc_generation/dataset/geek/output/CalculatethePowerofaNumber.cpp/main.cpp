
#include <systemc.h>

SC_MODULE(PowerCalculator) {
    SC_CTOR(PowerCalculator) {
        SC_THREAD(calculate_power);
    }

    void calculate_power() {
        sc_dt::sc_fixed<16, 8> x(6.1);  // Fixed-point representation for x
        sc_dt::sc_fixed<16, 8> y(4.8);  // Fixed-point representation for y
        sc_dt::sc_fixed<32, 16> result; // Larger fixed-point type to store result

        // Calculate power using a loop to approximate pow function
        result = 1.0;
        for (int i = 0; i < 1000; ++i) { // Simple approximation loop
            result *= x;
            x /= y;
        }
        
        // Print the result with two decimal places
        std::cout << "Result: " << result.to_double() << std::endl;
    }
};

int sc_main(int argc, char* argv[]) {
    PowerCalculator calc("PowerCalculator");
    sc_start();
    return 0;
}
