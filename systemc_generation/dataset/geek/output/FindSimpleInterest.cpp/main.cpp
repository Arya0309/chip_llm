
#include <systemc.h>

// Module to calculate Simple Interest
SC_MODULE(SimpleInterestCalculator) {
    // Input ports for principal, rate, and time
    sc_in<float> principal;
    sc_in<float> rate;
    sc_in<float> time;

    // Output port for simple interest
    sc_out<float> simple_interest;

    // Process to calculate simple interest
    void calculateSimpleInterest() {
        // Reading inputs
        float P = principal.read();
        float R = rate.read();
        float T = time.read();

        // Calculating simple interest
        float SI = (P * T * R) / 100.0f;

        // Writing output
        simple_interest.write(SI);
    }

    // Constructor to register the process
    SC_CTOR(SimpleInterestCalculator) {
        SC_METHOD(calculateSimpleInterest);
        sensitive << principal << rate << time;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with SimpleInterestCalculator
    sc_signal<float> principal;
    sc_signal<float> rate;
    sc_signal<float> time;
    sc_signal<float> simple_interest;

    // Instance of SimpleInterestCalculator
    SimpleInterestCalculator sic;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing inputs
        principal = 1.0f;
        rate = 1.0f;
        time = 1.0f;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Simple Interest = " << simple_interest.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : sic("sic") {
        // Connecting signals to the calculator ports
        sic.principal(principal);
        sic.rate(rate);
        sic.time(time);
        sic.simple_interest(simple_interest);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
