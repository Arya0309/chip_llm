
#include <systemc.h>

// Module to count the number of unique triplets whose XOR is 0
SC_MODULE(TripletCounter) {
    // Input port for the array size
    sc_in<int> n_in;
    
    // Input vector for the array elements
    sc_vector<sc_in<int>> a_in;
    
    // Output port for the count of triplets
    sc_out<int> count_out;
    
    // Internal storage for the array and count
    std::vector<int> a;
    int count;
    
    // Process to count the triplets
    void count_triplets() {
        // Read the array size
        int n = n_in.read();
        a.resize(n);
        
        // Read the array elements
        for (int i = 0; i < n; ++i) {
            a[i] = a_in[i].read();
        }
        
        // Create an unordered set to store unique elements
        std::unordered_set<int> s(a.begin(), a.end());
        
        // Initialize count to 0
        count = 0;
        
        // Traverse for all i, j pairs such that j > i
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                // XOR of a[i] and a[j]
                int xr = a[i] ^ a[j];
                
                // Check if xr is in the set and not equal to a[i] or a[j]
                if (s.find(xr) != s.end() && xr != a[i] && xr != a[j]) {
                    count++;
                }
            }
        }
        
        // Write the final count divided by 3
        count_out.write(count / 3);
    }
    
    // Constructor to register the process
    SC_CTOR(TripletCounter) : a_in("a_in", 0) {
        SC_METHOD(count_triplets);
        sensitive << n_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with TripletCounter
    sc_signal<int> n;
    sc_vector<sc_signal<int>> a;
    sc_signal<int> count;
    
    // Instance of TripletCounter
    TripletCounter counter;
    
    // Process to drive inputs and print outputs
    void drive_and_print() {
        // Initialize array size
        n = 6;
        a.init(n);
        
        // Initialize array elements
        a[0] = 1; a[1] = 3; a[2] = 5; a[3] = 10; a[4] = 14; a[5] = 15;
        
        // Wait for one delta cycle to let the counter process
        wait(1, SC_NS);
        
        // Print the result
        cout << "Number of unique triplets whose XOR is 0: " << count.read() << endl;
    }
    
    // Constructor to register the process
    SC_CTOR(Testbench) : counter("counter"), a("a", n.read()) {
        // Connecting signals to the counter ports
        counter.n_in(n);
        for (int i = 0; i < n.read(); ++i) {
            counter.a_in[i](a[i]);
        }
        counter.count_out(count);
        
        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
