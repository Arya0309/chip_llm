
#include <systemc.h>

SC_MODULE(RemoveLeadingZeros) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<sc_bv<12>> input_str; // Assuming a fixed length for demonstration
    sc_out<sc_bv<12>> output_str;

    SC_CTOR(RemoveLeadingZeros) {
        SC_METHOD(process);
        sensitive << clk.pos();
        dont_initialize();
    }

    void process() {
        if (reset.read()) {
            output_str.write(sc_bv<12>("000000000000"));
        } else {
            sc_bv<12> str = input_str.read();
            int i = 0;
            while (i < 12 && str[i] == '0') {
                i++;
            }
            sc_bv<12> result = str.range(11, i);
            result.range(11-i, 0) = str.range(11, i);
            result.range(11, 12-i) = "000000000000".range(11, 12-i);
            output_str.write(result);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    RemoveLeadingZeros rlz("rlz");
    sc_signal<bool> clk;
    sc_signal<bool> reset;
    sc_signal<sc_bv<12>> input_str;
    sc_signal<sc_bv<12>> output_str;

    rlz.clk(clk);
    rlz.reset(reset);
    rlz.input_str(input_str);
    rlz.output_str(output_str);

    // Simulation setup
    sc_start(0, SC_NS);
    reset.write(true);
    sc_start(1, SC_NS);
    reset.write(false);
    input_str.write("000001235690"); // Example input with fixed length
    sc_start(10, SC_NS);

    cout << "Processed String: " << output_str.read().to_string() << endl;

    return 0;
}
