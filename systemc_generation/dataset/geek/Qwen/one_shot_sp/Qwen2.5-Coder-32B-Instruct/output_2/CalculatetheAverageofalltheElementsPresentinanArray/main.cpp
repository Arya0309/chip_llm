
#include <systemc.h>

// AverageCalculator Module: Calculates the average of input data

SC_MODULE(AverageCalculator) {
    sc_out<double> average_out;          // Output port for the average
    sc_vector<sc_in<int>> data_in;       // Input vector for the data elements

    int num_elements;                     // Number of elements in the input data

    // Constructor: register the process
    SC_CTOR(AverageCalculator, int num_elements) 
        : data_in("data_in", num_elements), num_elements(num_elements) {
        SC_METHOD(calculate_average);
        sensitive << data_in;             // Sensitivity to all input data
    }

    // Method that calculates the average of the input data
    void calculate_average() {
        int sum = 0;
        for (int i = 0; i < num_elements; i++) {
            sum += data_in[i].read();     // Read each element and add to sum
        }
        double avg = static_cast<double>(sum) / num_elements;  // Calculate average
        average_out.write(avg);           // Write the average to the output port
    }
};

// Example usage of the AverageCalculator module


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
