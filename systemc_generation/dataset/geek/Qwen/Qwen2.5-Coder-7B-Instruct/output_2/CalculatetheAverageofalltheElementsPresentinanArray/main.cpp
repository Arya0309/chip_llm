
#include <systemc.h>


SC_MODULE(AverageCalculator) {
    sc_vector<sc_in<int>> data_in;  // Input port for the array elements
    sc_out<double> average_out;    // Output port for the average

    sc_signal<int> sum;             // Internal signal to store the sum

    // Constructor
    SC_CTOR(AverageCalculator) : data_in("data_in", 9), sum("sum") {
        // Process to calculate the sum
        SC_METHOD(calculate_sum);
        for (int i = 0; i < data_in.size(); i++) {
            sensitive << data_in[i];
        }

        // Process to calculate the average
        SC_METHOD(calculate_average);
        sensitive << sum;
    }

    // Method to calculate the sum of the array elements
    void calculate_sum() {
        int current_sum = 0;  // Initialize sum to zero
        for (int i = 0; i < data_in.size(); i++) {
            current_sum += data_in[i].read();
        }
        sum.write(current_sum);  // Write the new sum back to the signal
    }

    // Method to calculate the average
    void calculate_average() {
        average_out.write(static_cast<double>(sum) / data_in.size());
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
