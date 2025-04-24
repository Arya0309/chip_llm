scss
#include <systemc.h>

// Module that calculates the average of an array of integers

SC_MODULE(AverageCalculator) {
    // Input port for the array of integers
    sc_in<sc_vector<sc_in<int> > > data;
    // Input port for the number of array elements
    sc_in<int> num_elements;
    // Output port for the average
    sc_out<double> average;

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Process to calculate the average
        SC_METHOD(calculate_average);
        sensitive << data << num_elements;
    }

    // Method to calculate the average of the array
    void calculate_average() {
        // Initialize the sum to 0
        int sum = 0;
        // Iterate over the array elements
        for (int i = 0; i < num_elements.read(); i++) {
            // Add each element to the sum
            sum += data->read()[i].read();
        }
        // Calculate the average
        average.write((double)sum / num_elements.read());
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
