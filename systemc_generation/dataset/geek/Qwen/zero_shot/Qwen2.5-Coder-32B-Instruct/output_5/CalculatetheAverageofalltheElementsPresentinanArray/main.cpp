
#include <systemc.h>


SC_MODULE(AverageCalculator) {
    // Output port for the average
    sc_out<double> average_out;

    // Input vector for the data
    sc_vector< sc_in<int> > data_in;

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Sensitivity list: process triggered by changes in any data_in signal
        SC_METHOD(calculate_average);
        sensitive << data_in;
    }

    void calculate_average() {
        int sum = 0;
        int n = data_in.size();

        // Sum all the inputs
        for (int i = 0; i < n; i++) {
            sum += data_in[i].read();
        }

        // Calculate and write the average to the output port
        double avg = static_cast<double>(sum) / n;
        average_out.write(avg);
    }
};

// Example top-level module to instantiate and connect AverageCalculator

SC_MODULE(TopLevel) {
    sc_vector< sc_signal<int> > data_signals;
    sc_signal<double> average_signal;

    AverageCalculator calc;

    SC_CTOR(TopLevel) : calc("AverageCalc"), data_signals(9, "data_signals") {
        // Connect the data signals to the calculator's data_in ports
        for (int i = 0; i < 9; i++) {
            calc.data_in[i](data_signals[i]);
        }

        // Connect the calculator's average_out to the top-level average_signal
        calc.average_out(average_signal);

        // Initialize data signals with values
        SC_THREAD(initialize_data);
    }

    void initialize_data() {
        wait(1, SC_NS); // Wait a bit to ensure everything is initialized
        data_signals[0].write(10);
        data_signals[1].write(2);
        data_signals[2].write(3);
        data_signals[3].write(4);
        data_signals[4].write(5);
        data_signals[5].write(6);
        data_signals[6].write(7);
        data_signals[7].write(8);
        data_signals[8].write(9);
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
