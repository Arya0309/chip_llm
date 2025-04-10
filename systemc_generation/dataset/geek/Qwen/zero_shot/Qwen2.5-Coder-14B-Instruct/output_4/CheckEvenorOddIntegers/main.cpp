\n#include <systemc.h>\n\n
SC_MODULE(EvenOddChecker) {\n    sc_in<int> number_in;\n    sc_out<int> result_out;\n\n    SC_CTOR(EvenOddChecker) {\n        SC_METHOD(check_even_odd);\n        sensitive << number_in;\n    }\n\n    void check_even_odd() {\n        int n = number_in.read();\n        if (n % 2 == 0)\n            result_out.write(0); // 0 represents Even\n        else\n            result_out.write(1); // 1 represents Odd\n    }\n};\n\n

SC_MODULE(Testbench) {
    sc_signal<int> number_sig;   // Signal for the input number
    sc_signal<int> result_sig;   // Signal for the result

    EvenOddChecker checker_inst; // Instance of the EvenOddChecker module

    SC_CTOR(Testbench)
    : checker_inst("checker_inst")
    {
        checker_inst.number_in(number_sig);
        checker_inst.result_out(result_sig);
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case 1: Number 11 (Odd)
        number_sig.write(11);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 1
        assert(result_sig.read() == 1);

        // Test case 2: Number 12 (Even)
        number_sig.write(12);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 0
        assert(result_sig.read() == 0);
        
        std::cout << "All tests pass successfully!" << std::endl;
        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
