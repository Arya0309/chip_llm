
#include <systemc.h>

// Module to calculate the binomial coefficient
SC_MODULE(BinomialCoefficient) {
    sc_in<int> n;      // Input port for n
    sc_in<int> k;      // Input port for k
    sc_out<int> result; // Output port for the binomial coefficient

    // Constructor
    SC_CTOR(BinomialCoefficient) {
        // Process to compute the binomial coefficient
        SC_METHOD(compute_binomial_coeff);
        sensitive << n << k;
    }

    // Method to compute the binomial coefficient
    void compute_binomial_coeff() {
        int res = 1;
        int nk = n.read();
        int kk = k.read();
        if (kk > nk - kk)
            kk = nk - kk;
        for (int i = 0; i < kk; ++i) {
            res *= (nk - i);
            res /= (i + 1);
        }
        result.write(res);
    }
};

// Module to print Pascal's Triangle
SC_MODULE(PascalsTrianglePrinter) {
    sc_in<int> num_rows; // Input port for the number of rows
    sc_out<int> coeff;  // Output port for the binomial coefficient
    sc_out<bool> done;  // Output port to indicate completion

    BinomialCoefficient binom_coeff_inst; // Instance of the BinomialCoefficient module

    // Signals to connect between modules
    sc_signal<int> n_sig;
    sc_signal<int> k_sig;

    // Constructor
    SC_CTOR(PascalsTrianglePrinter)
        : binom_coeff_inst("binom_coeff_inst") {
        // Connect signals to ports
        binom_coeff_inst.n(n_sig);
        binom_coeff_inst.k(k_sig);
        binom_coeff_inst.result(coeff);

        // Process to print Pascal's Triangle
        SC_THREAD(print_pascals_triangle);
    }

    // Thread to print Pascal's Triangle
    void print_pascals_triangle() {
        int n = num_rows.read();
        for (int line = 0; line < n; line++) {
            for (int i = 0; i <= line; i++) {
                n_sig.write(line);
                k_sig.write(i);
                wait(1, SC_NS); // Wait for the binomial coefficient to be computed
                cout << " " << coeff.read();
            }
            cout << "\n";
        }
        done.write(true);
        sc_stop(); // Stop the simulation
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> num_rows_sig; // Signal for the number of rows
    sc_signal<int> coeff_sig;    // Signal for the binomial coefficient
    sc_signal<bool> done_sig;    // Signal to indicate completion

    PascalsTrianglePrinter printer_inst; // Instance of the PascalsTrianglePrinter module

    // Constructor
    SC_CTOR(Testbench)
        : printer_inst("printer_inst") {
        // Connect signals to ports
        printer_inst.num_rows(num_rows_sig);
        printer_inst.coeff(coeff_sig);
        printer_inst.done(done_sig);

        // Process to run the test
        SC_THREAD(run_test);
    }

    // Thread to run the test
    void run_test() {
        num_rows_sig.write(7); // Set the number of rows to 7
        wait(done_sig.posedge_event()); // Wait until the printer is done
        cout << "Pascal's Triangle printed successfully." << endl;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
