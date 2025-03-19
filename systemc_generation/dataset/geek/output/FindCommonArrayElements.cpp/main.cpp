
#include <systemc.h>
#include <algorithm>
#include <vector>

SC_MODULE(FindCommonElements) {
    SC_CTOR(FindCommonElements) {
        SC_THREAD(process);
    }

    void process() {
        // Define arrays
        int arr1[] = {1, 45, 54, 71, 76, 12};
        int arr2[] = {1, 7, 5, 4, 6, 12};

        // Compute the sizes
        int n1 = sizeof(arr1) / sizeof(arr1[0]);
        int n2 = sizeof(arr2) / sizeof(arr2[0]);

        // Sort the arrays
        sort(arr1, arr1 + n1);
        sort(arr2, arr2 + n2);

        // Vector to store result
        std::vector<int> v;

        // Finding common elements using set_intersection() function
        std::set_intersection(arr1, arr1 + n1, arr2, arr2 + n2, std::back_inserter(v));

        // Print the common elements
        for (auto i : v) {
            sc_core::sc_report_handler::report(sc_core::SC_INFO, "common_elements", ("Common Element: " + std::to_string(i)).c_str());
        }
    }
};

int sc_main(int argc, char* argv[]) {
    FindCommonElements find_common_elements("find_common_elements");
    sc_start();
    return 0;
}
