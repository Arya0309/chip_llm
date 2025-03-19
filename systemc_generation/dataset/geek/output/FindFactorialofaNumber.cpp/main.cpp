
#include <systemc.h>

SC_MODULE(FactorialCalculator) {
    SC_CTOR(FactorialCalculator) {
        SC_THREAD(calculate_factorial);
    }

    void calculate_factorial() {
        unsigned int num = 5;
        unsigned int res = 1;
        for (unsigned int i = 2; i <= num; ++i) {
            res *= i;
        }
        cout << "Factorial of " << num << " is " << res << endl;
        sc_stop(); // Stop simulation after calculation
    }
};

int sc_main(int argc, char* argv[]) {
    FactorialCalculator calc("FactorialCalc");
    sc_start();
    return 0;
}
