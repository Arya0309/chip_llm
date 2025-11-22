#include <systemc.h>

// ─────────────── ReverseDigits (DUT) ───────────────
SC_MODULE(ReverseDigits) {
    sc_in<int> num_in;       // 輸入數字
    sc_out<int> rev_out;     // 輸出反轉結果

    SC_CTOR(ReverseDigits) {
        SC_METHOD(do_reverse);
        sensitive << num_in;
        dont_initialize();
    }

    void do_reverse() {
        int num = num_in.read();
        int rev = 0;
        while (num > 0) {
            rev = rev * 10 + num % 10;
            num /= 10;
        }
        rev_out.write(rev);
    }
};

// ──────────────── Testbench ────────────────
SC_MODULE(Testbench) {
    sc_signal<int> num_sig, rev_sig;

    ReverseDigits dut{"ReverseDigits"};

    SC_CTOR(Testbench) {
        dut.num_in(num_sig);
        dut.rev_out(rev_sig);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        int input = 4562;
        int expected = 2654;

        num_sig.write(input);
        wait(1, SC_NS); // 等待 combinational logic 生效

        int result = rev_sig.read();
        std::cout << "Input: " << input << ", Reversed: " << result << std::endl;
        assert(result == expected);

        std::cout << "All tests passed successfully.\n";
        sc_stop();
    }
};

// ──────────────── sc_main ────────────────
int sc_main(int, char*[]) {
    Testbench tb{"tb"};
    sc_start();
    return 0;
}
