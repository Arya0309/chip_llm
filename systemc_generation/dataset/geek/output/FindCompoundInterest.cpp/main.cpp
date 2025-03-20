
#include <systemc.h>

// Module to calculate Compound Interest
SC_MODULE(CompoundInterestCalculator) {
    // Input ports for principal, rate, and time
    sc_in<double> principal;
    sc_in<double> rate;
    sc_in<double> time;
    
    // Output port for compound interest
    sc_out<double> compound_interest;

    // Process to calculate compound interest
    void calculateCompoundInterest() {
        // Reading inputs
        double p = principal.read();
        double r = rate.read();
        double t = time.read();

        // Calculating the compound interest
        double A = p * (pow((1 + r / 100), t));
        double CI = A - p;

        // Writing the result to the output port
        compound_interest.write(CI);
    }

    // Constructor to register the process
    SC_CTOR(CompoundInterestCalculator) {
        SC_METHOD(calculateCompoundInterest);
        sensitive << principal << rate << time;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with CompoundInterestCalculator
    sc_signal<double> principal;
    sc_signal<double> rate;
    sc_signal<double> time;
    sc_signal<double> compound_interest;

    // Instance of CompoundInterestCalculator
    CompoundInterestCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing values
        principal = 10000;
        rate = 5;
        time = 2;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Principal: " << principal.read() << endl;
        cout << "Rate: " << rate.read() << "%" << endl;
        cout << "Time: " << time.read() << " years" << endl;
        cout << "Compound interest is " << compound_interest.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.principal(principal);
        calculator.rate(rate);
        calculator.time(time);
        calculator.compound_interest(compound_interest);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
