
#include <systemc.h>

// Module to calculate and print Pascal's Triangle
SC_MODULE(PascalsTriangle) {
    sc_in<int> num_rows; // Input: number of rows to print
    sc_out<sc_bv<100>> triangle_output; // Output: string representation of the triangle

    // Internal method to calculate Pascal's Triangle
    void calculate_and_print_triangle() {
        int n = num_rows.read();
        std::ostringstream oss;

        // Iterate through every line and print entries in it
        for (int line = 0; line < n; line++) {
            // Every line has number of integers equal to line number
            for (int i = 0; i <= line; i++) {
                int coeff = binomialCoeff(line, i);
                oss << coeff << " ";
            }
            oss << "\n";
        }

        // Convert the string stream to a bit vector for output
        std::string result = oss.str();
        sc_bv<100> output(result.c_str());
        triangle_output.write(output);
    }

    // Function to calculate binomial coefficient
    int binomialCoeff(int n, int k) {
        int res = 1;
        if (k > n - k)
            k = n - k;
        for (int i = 0; i < k; ++i) {
            res *= (n - i);
            res /= (i + 1);
        }
        return res;
    }

    // Constructor to register the process
    SC_CTOR(PascalsTriangle) {
        SC_METHOD(calculate_and_print_triangle);
        sensitive << num_rows;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    sc_signal<int> num_rows_signal; // Signal to connect with PascalsTriangle
    sc_signal<sc_bv<100>> triangle_output_signal; // Signal to capture the output

    // Instance of PascalsTriangle
    PascalsTriangle pascal_triangle;

    // Process to drive inputs and print outputs
    void drive_and_print() {
        num_rows_signal = 7; // Set the number of rows to 7

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        sc_bv<100> result = triangle_output_signal.read();
        std::cout << result.to_string() << std::endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : pascal_triangle("pascal_triangle") {
        // Connecting signals to the calculator ports
        pascal_triangle.num_rows(num_rows_signal);
        pascal_triangle.triangle_output(triangle_output_signal);

        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
