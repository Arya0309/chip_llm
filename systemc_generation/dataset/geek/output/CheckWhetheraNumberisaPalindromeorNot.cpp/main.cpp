
#include <systemc.h>

SC_MODULE(PalindromeChecker) {
    sc_in<bool> clk;        // Clock signal
    sc_in<bool> rst_n;      // Active-low reset signal
    sc_out<bool> result;    // Output result: 1 if palindrome, 0 otherwise

    sc_signal<sc_uint<10>> index; // Index for looping through the string
    sc_signal<bool> done;         // Signal to indicate completion of checking
    sc_signal<bool> mismatch;     // Signal to indicate a mismatch was found

    SC_CTOR(PalindromeChecker) {
        SC_METHOD(check);
        sensitive << clk.pos() << rst_n.neg();

        index = 0;
        done = false;
        mismatch = false;
        result = false;
    }

    void check() {
        if (!rst_n.read()) { // Reset condition
            index = 0;
            done = false;
            mismatch = false;
            result = false;
        } else if (!done.read()) { // Normal operation
            static const char str[] = "112233445566778899000000998877665544332211";
            static const int len = sizeof(str) - 1; // Exclude null terminator

            if (index.read() < len / 2) {
                if (str[index.read()] != str[len - index.read() - 1]) {
                    mismatch = true;
                    done = true;
                } else {
                    index = index.read() + 1;
                }
            } else {
                done = true;
            }

            if (done.read() && !mismatch.read()) {
                result = true;
            } else if (done.read() && mismatch.read()) {
                result = false;
            }
        }
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS); // 10 ns clock period
    sc_signal<bool> rst_n("rst_n"); // Active-low reset
    sc_signal<bool> result("result"); // Result of palindrome check

    PalindromeChecker checker("checker");
    checker.clk(clk);
    checker.rst_n(rst_n);
    checker.result(result);

    // Initial reset
    rst_n.write(false);
    sc_start(10, SC_NS);
    rst_n.write(true);

    // Run simulation for some time
    sc_start(100, SC_NS);

    // Print result
    cout << "Is the string a palindrome? " << (result.read() ? "Yes" : "No") << endl;

    return 0;
}
