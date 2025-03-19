
#include <systemc.h>

SC_MODULE(CompoundInterestCalculator) {
    SC_CTOR(CompoundInterestCalculator) {
        SC_THREAD(calculate);
    }

    void calculate() {
        double principal = 10000.0; // Initial principal amount
        double rate = 5.0;          // Annual interest rate in percentage
        double time = 2.0;          // Time period in years

        // Calculate compound interest
        double A = principal * (pow((1 + rate / 100), time));
        double CI = A - principal;

        // Output the compound interest
        sc_report_handler::message(sc_core::SC_INFO, "COMPOUND_INTEREST",
                                   ("Compound interest is " + std::to_string(CI)).c_str());
    }
};

int sc_main(int argc, char* argv[]) {
    CompoundInterestCalculator cic("CompoundInterestCalculator");
    sc_start();
    return 0;
}
