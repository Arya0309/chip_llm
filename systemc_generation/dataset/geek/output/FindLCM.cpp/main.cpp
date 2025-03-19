
#include <systemc.h>

SC_MODULE(LCMCalculator) {
    sc_in<int> a; // Input port for first number
    sc_in<int> b; // Input port for second number
    sc_out<int> lcm; // Output port for LCM

    SC_CTOR(LCMCalculator) {
        SC_THREAD(calculate_lcm);
        sensitive << a << b;
        dont_initialize();
    }

    void calculate_lcm() {
        int max_num;
        int flag = 1;

        // Wait for valid inputs
        wait();

        // Determine the larger of the two numbers
        max_num = (a.read() > b.read()) ? a.read() : b.read();

        while (flag) {
            // Check if max_num is divisible by both a and b
            if (max_num % a.read() == 0 && max_num % b.read() == 0) {
                lcm.write(max_num);
                flag = 0; // Exit the loop
            } else {
                ++max_num; // Increment max_num and try again
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    LCMCalculator lcm_calc("lcm_calc");

    sc_signal<int> a_sig, b_sig, lcm_sig;

    lcm_calc.a(a_sig);
    lcm_calc.b(b_sig);
    lcm_calc.lcm(lcm_sig);

    a_sig.write(15); // Example input
    b_sig.write(20); // Example input

    sc_start();

    cout << "LCM of " << a_sig.read() << " and " << b_sig.read() << " is " << lcm_sig.read() << endl;

    return 0;
}
