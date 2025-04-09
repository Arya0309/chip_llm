
#include <systemc.h>

// AverageCalculator Module: Calculates the average of an array of integers

SC_MODULE(AverageCalculator) {
    sc_in<bool> clk;          // Clock input
    sc_in<int> num_elements;   // Number of elements in the array
    sc_vector<sc_in<int>> data_in;  // Input vector for the array elements
    sc_out<double> average_out;  // Output for the calculated average

    // Internal storage for the array
    int arr[9];  // Assuming maximum 9 elements for simplicity

    // Constructor: register the process
    SC_CTOR(AverageCalculator) {
        SC_THREAD(calculate_average);
        sensitive << clk.pos();
    }

    // Process that waits for the calculation request and computes the average
    void calculate_average() {
        while (true) {
            wait(); // Wait for a clock cycle
            if (num_elements.read() != 0) {
                // Copy data from input vector to internal array
                for (int i = 0; i < num_elements.read(); i++) {
                    arr[i] = data_in[i].read();
                }

                // Calculate the sum of the array elements
                int sum = 0;
                for (int i = 0; i < num_elements.read(); i++) {
                    sum += arr[i];
                }

                // Calculate and write the average to the output port
                average_out.write((double)sum / num_elements.read());

                // Wait for one cycle before allowing another calculation
                wait();
            }
        }
    }
};


SC_MODULE(Testbench) {
    //  9  sc_signal<int>，
    sc_vector< sc_signal<int> > data_sig;
    // 
    sc_signal<double> avg_sig;
    // AverageCalculator 
    AverageCalculator avg_calc_inst;

    // Constructor，
    SC_CTOR(Testbench)
    : data_sig("data_sig", 9),   //  9 
      avg_calc_inst("avg_calc_inst")
    {
        //  Testbench  AverageCalculator 
        for (int i = 0; i < 9; i++) {
            avg_calc_inst.data_in[i](data_sig[i]);
        }
        // 
        avg_calc_inst.average_out(avg_sig);

        // 
        SC_THREAD(run_test);
    }

    // ：、、
    void run_test() {
        // 
        int test_data[9] = {10, 2, 3, 4, 5, 6, 7, 8, 9};
        for (int i = 0; i < 9; i++) {
            data_sig[i].write(test_data[i]);
        }
        // 
        wait(1, SC_NS);

        // 
        double computed_avg = avg_sig.read();
        std::cout << "Computed average: " << computed_avg << std::endl;
        assert(computed_avg == 6.0);

        // 
        int test_data2[9] = {1, 2, 3, 4, 5, 6, 7, 8, 10};
        for (int i = 0; i < 9; i++) {
            data_sig[i].write(test_data2[i]);
        }
        // 
        wait(1, SC_NS);

        // 
        computed_avg = avg_sig.read();
        std::cout << "Computed average: " << computed_avg << std::endl;
        double epsilon = 1e-5; // 
        assert(fabs(computed_avg - 5.11111) < epsilon); // 

        std::cout << "Test passed successfully." << std::endl;

        // 
        sc_stop();
    }
};

// sc_main：，
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
