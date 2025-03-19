
#include <systemc.h>

SC_MODULE(FibonacciEvenSum) {
    sc_in<int> n_in;
    sc_out<int> sum_out;
    
    void calculate_even_sum() {
        int n = n_in.read();
        if (n <= 0) {
            sum_out.write(0);
            return;
        }

        int fibo[2 * n + 1];
        fibo[0] = 0, fibo[1] = 1;

        int sum = 0;

        for (int i = 2; i <= 2 * n; i++) {
            fibo[i] = fibo[i - 1] + fibo[i - 2];
            if (i % 2 == 0)
                sum += fibo[i];
        }

        sum_out.write(sum);
    }

    SC_CTOR(FibonacciEvenSum) {
        SC_METHOD(calculate_even_sum);
        sensitive << n_in;
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> n;
    sc_signal<int> sum;

    FibonacciEvenSum fib("fib");
    fib.n_in(n);
    fib.sum_out(sum);

    n.write(8); // Set the input value for n
    sc_start();

    cout << "Even indexed Fibonacci Sum upto " << n.read() << " terms: " << sum.read() << endl;

    return 0;
}
