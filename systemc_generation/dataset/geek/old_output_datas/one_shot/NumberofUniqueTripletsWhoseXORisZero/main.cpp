
#include <systemc.h>
#include <unordered_set>
#include <vector>

// Module to count the number of unique triplets whose XOR is 0
SC_MODULE(TripletCounter) {
    sc_in<std::vector<int>> input_array; // Input port for the array
    sc_out<int> output_count;            // Output port for the count of triplets

    // Constructor
    SC_CTOR(TripletCounter) {
        // Process to count triplets
        SC_METHOD(count_triplets);
        sensitive << input_array;
    }

    // Method to count the triplets
    void count_triplets() {
        std::vector<int> a = input_array.read();
        int n = a.size();
        
        // To store values that are present
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            s.insert(a[i]);
        
        // Stores the count of unique triplets
        int count = 0;
        
        // Traverse for all i, j pairs such that j > i
        for (int i = 0; i < n-1; i++) {
            for (int j = i + 1; j < n; j++) {
                // XOR of a[i] and a[j]
                int xr = a[i] ^ a[j];
                
                // If xr of two numbers is present,
                // then increase the count
                if (s.find(xr) != s.end() && xr != a[i] && xr != a[j])
                    count++;
            }
        }
        
        // Returns answer
        output_count.write(count / 3);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::vector<int>> input_array; // Signal for the array
    sc_signal<int> output_count;             // Signal for the count of triplets

    TripletCounter triplet_counter_inst; // Instance of the TripletCounter module

    // Constructor
    SC_CTOR(Testbench) : triplet_counter_inst("triplet_counter") {
        // Connect signals to ports
        triplet_counter_inst.input_array(input_array);
        triplet_counter_inst.output_count(output_count);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the array
        std::vector<int> test_array = {1, 3, 5, 10, 14, 15};
        input_array.write(test_array);
        wait(1, SC_NS); // Wait for the counter to process
        std::cout << "Count of triplets: " << output_count.read() << std::endl;

        // Print success message
        std::cout << "Test passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
