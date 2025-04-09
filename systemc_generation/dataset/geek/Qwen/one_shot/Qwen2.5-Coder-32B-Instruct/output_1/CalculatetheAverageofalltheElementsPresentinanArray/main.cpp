
#include <systemc.h>

// Define a constant for the number of elements in the array
static const int NUM_ELEMENTS = 9;

// SystemC module to calculate the average of an array

SC_MODULE(AverageCalculator) {
    // Input ports for the array elements
    sc_vector< sc_in<int> > data_in;
    // Output port for the calculated average
    sc_out<double> average_out;

    // Constructor with sensitivity list
    SC_CTOR(AverageCalculator) : data_in("data_in", NUM_ELEMENTS) {
        // Register the process that calculates the average
        SC_METHOD(calculate_average);
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            sensitive << data_in[i]; // Sensitivity to all input data elements
        }
    }

    // Method to calculate the average
    void calculate_average() {
        int sum = 0;
        // Sum up all the elements from the input vector
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            sum += data_in[i]->read();
        }
        // Calculate the average and write it to the output port
        double avg = static_cast<double>(sum) / NUM_ELEMENTS;
        average_out.write(avg);
    }
};

// Example usage


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
