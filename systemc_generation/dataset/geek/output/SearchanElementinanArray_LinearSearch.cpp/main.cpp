
#include <systemc.h>

SC_MODULE(LinearSearch) {
    sc_in<sc_uint<8>> key;  // Input key to search
    sc_in<bool> start_search;  // Signal to start search
    sc_out<sc_int<8>> result;  // Output result of search (-1 if not found)
    sc_vector<sc_in<sc_uint<8>>> vec;  // Input vector of integers

    SC_CTOR(LinearSearch) : vec("vec", 8) {  // Constructor initializing vector size
        SC_METHOD(search);
        sensitive << start_search.pos();
    }

    void search() {
        if (start_search.read()) {
            bool found = false;
            int pos = -1;
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i].read() == key.read()) {
                    pos = i + 1;  // Convert to 1-based index
                    found = true;
                    break;
                }
            }
            if (!found) pos = -1;
            result.write(pos);
        }
    }
};

int sc_main(int argc, char* argv[]) {
    LinearSearch ls("ls");
    sc_signal<sc_uint<8>> key;
    sc_signal<bool> start_search;
    sc_signal<sc_int<8>> result;
    sc_vector<sc_signal<sc_uint<8>>> vec("vec", 8);

    ls.key(key);
    ls.start_search(start_search);
    ls.result(result);
    for (int i = 0; i < 8; i++) {
        ls.vec[i](vec[i]);
    }

    // Initialize vector values
    vec[0].write(1);
    vec[1].write(2);
    vec[2].write(3);
    vec[3].write(4);
    vec[4].write(5);
    vec[5].write(8);
    vec[6].write(9);
    vec[7].write(11);

    // Set key to search
    key.write(8);
    start_search.write(false);
    sc_start(1, SC_NS);  // Wait for one cycle

    // Start search
    start_search.write(true);
    sc_start(1, SC_NS);  // Wait for one cycle

    // Print result
    if (result.read() != -1)
        cout << key.read() << " Found at Position: " << result.read() << endl;
    else
        cout << key.read() << " NOT found." << endl;

    return 0;
}
