
#include <systemc.h>

SC_MODULE(Reverser) {
    sc_in<int> num_in;  // Input port for the number to be reversed
    sc_out<int> num_out; // Output port for the reversed number

    void reverse_process() {
        int num = num_in.read();
        int rev_num = 0;

        while (num > 0) {
            rev_num = rev_num * 10 + num % 10;
            num = num / 10;
        }

        num_out.write(rev_num);
    }

    SC_CTOR(Reverser) {
        SC_METHOD(reverse_process);
        sensitive << num_in;
    }
};

int sc_main(int argc, char* argv[]) {
    Reverser reverser("reverser");
    sc_signal<int> num_sig;
    sc_signal<int> result_sig;

    reverser.num_in(num_sig);
    reverser.num_out(result_sig);

    num_sig.write(4562);

    sc_start();

    std::cout << "Reverse of num is " << result_sig.read() << std::endl;

    return 0;
}
