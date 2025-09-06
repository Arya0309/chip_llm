#include <systemc.h>

// ─────────────── PowerFunction (DUT) ───────────────
SC_MODULE(PowerFunction) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;

    sc_in<int> base;
    sc_in<int> exponent;
    sc_out<int> result;

    int internal_result;

    SC_CTOR(PowerFunction) {
        SC_THREAD(compute_proc);
        sensitive << clk.pos();
    }

    void compute_proc() {
        while (true) {
            wait();  // Wait for clock edge
            if (start.read()) {
                int a = base.read();
                int b = exponent.read();

                if (b == 0) {
                    internal_result = 1;
                } else {
                    int answer = a;
                    int increment = a;
                    for (int i = 1; i < b; ++i) {
                        for (int j = 1; j < a; ++j) {
                            answer += increment;
                        }
                        increment = answer;
                    }
                    internal_result = answer;
                }

                result.write(internal_result);

                // Signal done for one cycle
                done.write(true);
                wait();
                done.write(false);
            }
        }
    }
};

// ──────────────── Testbench ────────────────
SC_MODULE(Testbench) {
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> start, done;
    sc_signal<int> base, exponent, result;

    PowerFunction* dut;

    SC_CTOR(Testbench) {
        // DUT instantiation and port binding
        dut = new PowerFunction("PowerFunction");
        dut->clk(clk);
        dut->start(start);
        dut->done(done);
        dut->base(base);
        dut->exponent(exponent);
        dut->result(result);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case: 5^3 = 125
        base.write(5);
        exponent.write(3);

        wait(clk.posedge_event());
        start.write(true);
        wait(clk.posedge_event());
        start.write(false);

        // Wait for done signal
        wait(done.posedge_event());

        int res = result.read();
        std::cout << "Result: " << res << std::endl;
        assert(res == 125);

        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

// ──────────────── sc_main ────────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
