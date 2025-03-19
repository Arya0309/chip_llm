
#include <systemc.h>

SC_MODULE(PowerModule) {
    sc_in<int> a; // Input port for base
    sc_in<int> b; // Input port for exponent
    sc_out<int> result; // Output port for result

    void compute_pow() {
        int local_a = a.read();
        int local_b = b.read();
        
        if (local_b == 0) {
            result.write(1);
            return;
        }
        
        int answer = local_a;
        int increment = local_a;
        
        for (int i = 1; i < local_b; i++) {
            for (int j = 1; j < local_a; j++) {
                answer += increment;
            }
            increment = answer;
        }
        
        result.write(answer);
    }

    SC_CTOR(PowerModule) {
        SC_METHOD(compute_pow);
        sensitive << a << b;
    }
};

int sc_main(int argc, char* argv[]) {
    PowerModule power("power");
    sc_signal<int> a_sig, b_sig, result_sig;

    power.a(a_sig);
    power.b(b_sig);
    power.result(result_sig);

    a_sig.write(5);
    b_sig.write(3);

    sc_start();

    cout << "Result: " << result_sig.read() << endl;

    return 0;
}
