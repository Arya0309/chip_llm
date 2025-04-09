
#include <systemc.h>

// AverageCalculator Module: Implements the average calculation

SC_MODULE(AverageCalculator) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start calculating average
    sc_out<double> average_out;  // Output for the calculated average
    sc_vector<sc_in<int>> data_in; // Input vector for the data array

    // Internal storage for the array (dynamic size)
    int* arr;
    int n;

    // Constructor: register the process
    SC_CTOR(AverageCalculator) : data_in("data_in", 9) {
        SC_THREAD(average_process);
        sensitive << clk.pos();
    }

    // Destructor: clean up dynamically allocated memory
    ~AverageCalculator() {
        delete[] arr;
    }

    // Process that waits for the start signal and calculates the average
    void average_process() {
        while (true) {
            wait(); // Wait for a clock cycle
            if (start.read() == true) {
                // Load the input data into the internal array
                for (int i = 0; i < n; i++) {
                    arr[i] = data_in[i].read();
                }

                // Calculate the sum of the array elements
                int sum = 0;
                for (int i = 0; i < n; i++) {
                    sum += arr[i];
                }

                // Calculate and write the average to the output port
                average_out.write((double)sum / n);

                // Indicate that the calculation is complete
                wait(); // Wait for one cycle to signal completion
            }
        }
    }

    // Helper method to initialize the internal array and set the size
    void init_array(int size) {
        n = size;
        arr = new int[n];
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
