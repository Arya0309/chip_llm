scss
#include <systemc.h>

// Module that calculates the average of an array

SC_MODULE(AverageCalculator) {
    sc_vector<sc_in<int> > data_in; // Vector of input data
    sc_out<double> average_out; // Output port for the average

    SC_CTOR(AverageCalculator) {
        // Declare input ports
        for (int i = 0; i < data_in.size(); i++) {
            data_in[i] = sc_in<int>::get_new_port();
        }
    }

    // calculate the average and write it to the output
    void calculate_average() {
        double sum = 0;
        for (int i = 0; i < data_in.size(); i++) {
            sum += data_in[i].read();
        }
        average_out.write(sum / data_in.size());
    }
};

// Module that drives the averager

SC_MODULE(Driver) {
    sc_vector<sc_out<int> > data_out; // Vector of output data

    SC_CTOR(Driver) {
        // Create output ports
        for (int i = 0; i < data_out.size(); i++) {
            data_out[i] = sc_out<int>::get_new_port();
        }
    }

    void drive() {
        // Driver code to generate the input data
        for (int i = 0; i < data_out.size(); i++) {
            data_out[i].write(i);
        }
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
