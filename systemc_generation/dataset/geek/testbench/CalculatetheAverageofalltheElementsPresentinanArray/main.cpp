#include <systemc.h>

// AverageCalculator module: computes the average of nine integers
SC_MODULE(AverageCalculator) {
    sc_in<int> data0, data1, data2, data3, data4, data5, data6, data7, data8;
    sc_out<double> average_out;

    SC_CTOR(AverageCalculator) {
        SC_METHOD(compute_average);
        dont_initialize();
        sensitive << data0 << data1 << data2
                  << data3 << data4 << data5
                  << data6 << data7 << data8;
    }

    void compute_average() {
        int sum = 0;
        sum += data0.read();
        sum += data1.read();
        sum += data2.read();
        sum += data3.read();
        sum += data4.read();
        sum += data5.read();
        sum += data6.read();
        sum += data7.read();
        sum += data8.read();
        average_out.write(static_cast<double>(sum) / 9.0);
    }
};

// Testbench module: provides inputs and checks the computed average
SC_MODULE(Testbench) {
    sc_signal<int> sig_data0, sig_data1, sig_data2,
                   sig_data3, sig_data4, sig_data5,
                   sig_data6, sig_data7, sig_data8;
    sc_signal<double> avg_sig;

    AverageCalculator avg_calc_inst;

    SC_CTOR(Testbench)
    : avg_calc_inst("avg_calc_inst")
    {
        avg_calc_inst.data0(sig_data0);
        avg_calc_inst.data1(sig_data1);
        avg_calc_inst.data2(sig_data2);
        avg_calc_inst.data3(sig_data3);
        avg_calc_inst.data4(sig_data4);
        avg_calc_inst.data5(sig_data5);
        avg_calc_inst.data6(sig_data6);
        avg_calc_inst.data7(sig_data7);
        avg_calc_inst.data8(sig_data8);
        avg_calc_inst.average_out(avg_sig);

        SC_THREAD(run_test);
    }

    void run_test() {
        int test_data1[9] = {10, 2, 3, 4, 5, 6, 7, 8, 9};
        sig_data0.write(test_data1[0]);
        sig_data1.write(test_data1[1]);
        sig_data2.write(test_data1[2]);
        sig_data3.write(test_data1[3]);
        sig_data4.write(test_data1[4]);
        sig_data5.write(test_data1[5]);
        sig_data6.write(test_data1[6]);
        sig_data7.write(test_data1[7]);
        sig_data8.write(test_data1[8]);
        wait(1, SC_NS);
        double avg1 = avg_sig.read();
        std::cout << "Computed average 1: " << avg1 << std::endl;
        assert(fabs(avg1 - 6.0) < 1e-9);

        int test_data2[9] = {1, 2, 3, 4, 5, 6, 7, 8, 10};
        sig_data0.write(test_data2[0]);
        sig_data1.write(test_data2[1]);
        sig_data2.write(test_data2[2]);
        sig_data3.write(test_data2[3]);
        sig_data4.write(test_data2[4]);
        sig_data5.write(test_data2[5]);
        sig_data6.write(test_data2[6]);
        sig_data7.write(test_data2[7]);
        sig_data8.write(test_data2[8]);
        wait(1, SC_NS);
        double avg2 = avg_sig.read();
        std::cout << "Computed average 2: " << avg2 << std::endl;
        assert(fabs(avg2 - 5.11111) < 1e-5);

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
