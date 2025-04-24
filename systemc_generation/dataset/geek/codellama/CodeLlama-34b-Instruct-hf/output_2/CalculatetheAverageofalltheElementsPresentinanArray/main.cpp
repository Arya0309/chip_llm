
#include <systemc.h>

// Module that calculates the average of an array of integers

SC_MODULE(AverageCalculator) {
    // Inputs
    sc_vector<sc_in<int>> data_in; // Input port for the array of integers
    sc_in<size_t> n;               // Input port for the size of the array
    
    // Output
    sc_out<double> average_out;  // Output port for the average of the array elements
    
    // Constructor
    SC_CTOR(AverageCalculator) {
        // Process to calculate the average
        SC_METHOD(calculate_average);
        sensitive << data_in << n;
    }
    
    // Method to calculate the average
    void calculate_average() {
        // Calculate the sum of the array elements
        int sum = 0;
        for (size_t i = 0; i < n.read(); i++) {
            sum += data_in[i].read();
        }
        // Calculate the average
        double average = (double)sum / n.read();
        // Write the output to the port
        average_out.write(average);
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
