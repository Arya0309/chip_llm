
#include <systemc.h>

// AverageCalculator Module: Implements the average calculation

SC_MODULE(AverageCalculator) {
    sc_out<double> average_out; // Output port for the average
    sc_vector< sc_in<int> > data_in; // Input vector for the data array

    // Internal storage for the array size
    int n;

    // Constructor: register the process
    SC_CTOR(AverageCalculator) : data_in("data_in", n) {
        SC_METHOD(calculate_average);
        sensitive << data_in;
    }

    // Method that calculates the average
    void calculate_average() {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += data_in[i].read();
        }
        double avg = static_cast<double>(sum) / n;
        average_out.write(avg);
    }
};

// Example top-level module to demonstrate usage

SC_MODULE(TopLevel) {
    sc_signal<double> average_result;
    sc_vector< sc_signal<int> > data_signals;

    AverageCalculator avg_calc;

    SC_CTOR(TopLevel) : data_signals("data_signals", 9), avg_calc("avg_calc") {
        avg_calc.average_out(average_result);
        for (int i = 0; i < 9; ++i) {
            avg_calc.data_in[i](data_signals[i]);
        }

        // Initialize data signals
        SC_THREAD(initialize_data);
    }

    void initialize_data() {
        int data[] = {10, 2, 3, 4, 5, 6, 7, 8, 9};
        for (int i = 0; i < 9; ++i) {
            data_signals[i].write(data[i]);
        }
        wait(1); // Wait for one delta cycle to allow calculation
        sc_stop(); // Stop simulation after calculation
    }
};

// Main function to run the simulation


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
