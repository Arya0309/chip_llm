
#include <systemc.h>

// Module that calculates the average of an array of integers

SC_MODULE(AverageCalculator) {
    sc_in<int> data_in;  // Input port for the array of integers
    sc_out<double> average_out; // Output port for the average

    // Function to calculate the average of an array of integers
    void average(sc_vector<sc_in<int>> data) {
        // Find the sum of the array elements
        int sum = 0;
        for (int i = 0; i < data.size(); i++) {
            sum += data[i].read();
        }
        // Calculate the average
        average_out.write((double)sum / data.size());
    }

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Map the input and output ports to the function
        data_in.bind(data);
        average_out.bind(average);
    }
};

// Main function to demonstrate how to use the AverageCalculator module
int main() {
    sc_vector<sc_in<int>> data = { 10, 2, 3, 4, 5, 6, 7, 8, 9 };
    sc_out<double> average_out;

    // Instantiate the AverageCalculator module
    AverageCalculator average_inst("average_inst");

    // Connect the inputs and outputs of the module
    data_in.bind(average_inst.data_in);
    average_out.bind(average_inst.average_out);

    // Run the simulation
    sc_start(100, SC_NS);

    return 0;
}


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
