#include <systemc.h>

// Module that adds two binary values represented as bit-vectors
SC_MODULE(BinaryAdderBV) {
    sc_in< sc_dt::sc_bv<8> > A;      // 8-bit input A
    sc_in< sc_dt::sc_bv<8> > B;      // 8-bit input B
    sc_out< sc_dt::sc_bv<9> > SUM;   // 9-bit sum (含進位)

    SC_CTOR(BinaryAdderBV) {
        SC_METHOD(do_add);
        sensitive << A << B;
    }

    void do_add() {
        // 讀取 bit-vector，轉成 unsigned
        unsigned a = A.read().to_uint();
        unsigned b = B.read().to_uint();
        unsigned s = a + b;
        // 將結果轉回固定寬度的 bit-vector
        sc_dt::sc_bv<9> result(s);
        SUM.write(result);
    }
};

// Testbench 模組
SC_MODULE(Testbench) {
    sc_signal< sc_dt::sc_bv<8> > sig_a, sig_b;
    sc_signal< sc_dt::sc_bv<9> > sig_sum;

    BinaryAdderBV adder;  // 實例化加法器

    SC_CTOR(Testbench)
    : adder("adder") {
        adder.A(sig_a);
        adder.B(sig_b);
        adder.SUM(sig_sum);
        SC_THREAD(run_tests);
    }

    void run_tests() {
        auto trim = [&](const std::string &bv) {
            auto pos = bv.find('1');
            if (pos == std::string::npos) return std::string("0");
            return bv.substr(pos);
        };

        // Test case 1: "1101" + "100" => "10001"
        sig_a.write("00001101");
        sig_b.write("00000100");
        wait(1, SC_NS);
        {
            std::string s = sig_sum.read().to_string();
            std::string output = trim(s);
            std::cout << "1101 + 100 = 10001" << std::endl;
            std::cout << "Output: " << output << std::endl;
            assert(output == "10001");
        }

        // Test case 2: "1010" + "11010" => "100100"
        sig_a.write("00001010");
        sig_b.write("00011010");
        wait(1, SC_NS);
        {
            std::string s = sig_sum.read().to_string();
            std::string output = trim(s);
            std::cout << "Test case 2: 1010 + 11010 = 100100" << std::endl;
            std::cout << "Output: " << output << std::endl;
            assert(output == "100100");
        }

        // Test case 3: "0" + "0" => "0"
        sig_a.write("00000000");
        sig_b.write("00000000");
        wait(1, SC_NS);
        {
            std::string s = sig_sum.read().to_string();
            std::string t = trim(s);
            std::cout << "0 + 0 = " << t << std::endl;
            assert(t == "0");
        }

        // Test case 4: "1111" + "1" => "10000"
        sig_a.write("00001111");
        sig_b.write("00000001");
        wait(1, SC_NS);
        {
            std::string s = sig_sum.read().to_string();
            std::string t = trim(s);
            std::cout << "1111 + 1 = " << t << std::endl;
            assert(t == "10000");
        }

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

// sc_main
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
