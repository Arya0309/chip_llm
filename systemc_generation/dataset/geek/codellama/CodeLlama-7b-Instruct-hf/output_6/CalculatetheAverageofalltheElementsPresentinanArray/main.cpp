c
#include <systemc.h>

// Module that calculates the average of an array

SC_MODULE(AverageCalculator) {
    // Input ports
    sc_in<sc_vector<int>> arr_in;  // Array input
    sc_out<double> average_out;  // AverageCalculator output

    // Function to calculate the average of an array
    void average(void) {
        // Calculate the array size
        int n = arr_in.read().size();

        // Initialize the accumulator variable
        int sum = 0;

        // Iterate over the array and add each element to sum
        for (int i = 0; i < n; i++) {
            sum += arr_in.read()[i];
        }

        // Divide sum by the size of the array to get the average
        average_out.write((double)sum / n);
    }

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Register the average function with the main process
        SC_METHOD(average);
        sensitive << arr_in;
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
