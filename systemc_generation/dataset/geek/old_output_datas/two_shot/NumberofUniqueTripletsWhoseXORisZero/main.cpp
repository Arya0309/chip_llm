
#include <systemc.h>
#include <unordered_set>

// Module to count the number of unique triplets whose XOR is 0
SC_MODULE(TripletCounter) {
    sc_in<sc_uint<32>> a[6];  // Input ports for array elements
    sc_out<sc_uint<32>> count_out; // Output port for the count of triplets

    // Constructor
    SC_CTOR(TripletCounter) {
        // Process to count triplets
        SC_METHOD(count_triplets);
        sensitive << a[0] << a[1] << a[2] << a[3] << a[4] << a[5];
    }

    // Method to count the triplets
    void count_triplets() {
        // Array to hold input values
        int arr[6];
        for (int i = 0; i < 6; ++i) {
            arr[i] = a[i].read();
        }

        // To store values that are present
        std::unordered_set<int> s;
        for (int i = 0; i < 6; i++)
            s.insert(arr[i]);

        // Stores the count of unique triplets
        int count = 0;

        // Traverse for all i, j pairs such that j > i
        for (int i = 0; i < 5; i++) {
            for (int j = i + 1; j < 6; j++) {
                // XOR of a[i] and a[j]
                int xr = arr[i] ^ arr[j];

                // If xr of two numbers is present,
                // then increase the count
                if (s.find(xr) != s.end() && xr != arr[i] && xr != arr[j])
                    count++;
            }
        }

        // Returns answer
        count_out.write(count / 3);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_uint<32>> a[6]; // Signals for array elements
    sc_signal<sc_uint<32>> count_out; // Signal for the count of triplets

    TripletCounter triplet_counter_inst; // Instance of the TripletCounter module

    // Constructor
    SC_CTOR(Testbench) : triplet_counter_inst("triplet_counter") {
        // Connect signals to ports
        for (int i = 0; i < 6; ++i) {
            triplet_counter_inst.a[i](a[i]);
        }
        triplet_counter_inst.count_out(count_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array values
        a[0] = 1; a[1] = 3; a[2] = 5; a[3] = 10; a[4] = 14; a[5] = 15;
        wait(1, SC_NS); // Wait for the process to update

        // Print the result
        std::cout << "Number of unique triplets: " << count_out.read() << std::endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
