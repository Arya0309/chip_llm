
#include <systemc.h>

SC_MODULE(SumCalculator) {
    sc_in<int> n;        // Input port for n
    sc_out<int> sum;     // Output port for the sum

    void calculate_sum() {
        int local_sum = 0;
        for (int i = 1; i <= n.read(); i++) {
            local_sum += i;
        }
        sum.write(local_sum);
    }

    SC_CTOR(SumCalculator) {
        SC_METHOD(calculate_sum);
        sensitive << n;   // Method triggered when 'n' changes
    }
};

int sc_main(int argc, char* argv[]) {
    SumCalculator calc("calc");
    sc_signal<int> n_sig;
    sc_signal<int> sum_sig;

    calc.n(n_sig);
    calc.sum(sum_sig);

    n_sig = 5;  // Set the input value

    sc_start();

    cout << "Sum of first " << n_sig.read() << " natural numbers: " << sum_sig.read() << endl;

    return 0;
}
