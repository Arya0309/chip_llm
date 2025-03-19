
#include <systemc.h>

SC_MODULE(ArmstrongChecker) {
    sc_in<int> n_in;  // Input port for the number to check
    sc_out<bool> is_armstrong;  // Output port indicating if the number is an Armstrong number

    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(check_armstrong);
        sensitive << n_in;
    }

    void check_armstrong() {
        int n = n_in.read();
        int temp = n;
        int p = 0;

        while (n > 0) {
            int rem = n % 10;
            p = p + (rem * rem * rem);
            n = n / 10;
        }

        if (temp == p) {
            is_armstrong.write(true);
        } else {
            is_armstrong.write(false);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> n;
    sc_signal<bool> result;

    ArmstrongChecker checker("checker");
    checker.n_in(n);
    checker.is_armstrong(result);

    // Test the module with the number 153
    n.write(153);
    sc_start();

    if (result.read()) {
        cout << "Yes. It is Armstrong No." << endl;
    } else {
        cout << "No. It is not an Armstrong No." << endl;
    }

    return 0;
}
