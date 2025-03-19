
#include <systemc.h>

SC_MODULE(PrimeFinder) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<sc_uint<32>> prime_number;
    sc_out<bool> valid;

    SC_CTOR(PrimeFinder) {
        SC_METHOD(find_primes);
        sensitive << clk.pos();
        dont_initialize();
    }

    void find_primes() {
        static int a = 100, b = 300, i = 100, j, flag;
        static bool output_ready = false;

        if (rst.read()) {
            i = a;
            output_ready = false;
        } else {
            if (!output_ready) {
                if (i > b) {
                    valid.write(false);
                    return;
                }
                if (i == 1 || i == 0) {
                    i++;
                    return;
                }
                flag = 1;
                for (j = 2; j <= i / 2; ++j) {
                    if (i % j == 0) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    prime_number.write(i);
                    valid.write(true);
                    output_ready = true;
                } else {
                    i++;
                }
            } else {
                output_ready = false;
                i++;
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> rst;
    sc_signal<sc_uint<32>> prime_number;
    sc_signal<bool> valid;

    PrimeFinder prime_finder("prime_finder");
    prime_finder.clk(clk);
    prime_finder.rst(rst);
    prime_finder.prime_number(prime_number);
    prime_finder.valid(valid);

    rst.write(true);
    sc_start(20, SC_NS);
    rst.write(false);

    while (true) {
        sc_start(10, SC_NS);
        if (valid.read()) {
            cout << "Prime number: " << prime_number.read() << endl;
        }
        if (prime_number.read() >= 293) break;
        // if (prime_number.read() >= 300) break;
    }

    sc_start(10, SC_NS);
    return 0;
}
