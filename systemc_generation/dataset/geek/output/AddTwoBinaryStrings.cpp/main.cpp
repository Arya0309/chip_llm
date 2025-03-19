
#include <systemc.h>

SC_MODULE(BinaryAdder) {
    sc_in<sc_bv<32>> a_in;  // Input port for first binary number
    sc_in<sc_bv<32>> b_in;  // Input port for second binary number
    sc_out<sc_bv<33>> sum_out;  // Output port for the sum (one extra bit for carry)

    SC_CTOR(BinaryAdder) {
        SC_METHOD(add);
        sensitive << a_in << b_in;
    }

    void add() {
        sc_bv<32> a = a_in.read();
        sc_bv<32> b = b_in.read();
        sc_bv<33> sum = 0;  // Initialize sum with an extra bit for carry

        bool carry = false;

        // Perform binary addition
        for (int i = 0; i < 32; ++i) {
            bool bit_a = a[i];
            bool bit_b = b[i];
            bool bit_sum = bit_a ^ bit_b ^ carry;
            carry = (bit_a & bit_b) | (carry & (bit_a ^ bit_b));

            sum[i] = bit_sum;
        }
        sum[32] = carry;  // Set the most significant bit as carry

        sum_out.write(sum);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_bv<32> a = "1101";  // Example input 1
    sc_bv<32> b = "100";   // Example input 2

    BinaryAdder adder("adder");
    sc_signal<sc_bv<32>> a_sig, b_sig;
    sc_signal<sc_bv<33>> sum_sig;

    adder.a_in(a_sig);
    adder.b_in(b_sig);
    adder.sum_out(sum_sig);

    a_sig = a;
    b_sig = b;

    sc_start();

    std::cout << "Sum: " << sum_sig.read().to_string().substr(1) << std::endl;  // Print the sum, excluding the carry bit

    return 0;
}
