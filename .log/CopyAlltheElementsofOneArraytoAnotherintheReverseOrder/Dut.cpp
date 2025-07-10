#include "Dut.h"

void printArray(int arr[], int len) {
	int i;
	for (i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
}

SC_MODULE(Dut) {
	sc_in_clk i_clk;
	sc_in<bool> i_rst;

	sc_fifo_in<int> i_arr;
	sc_fifo_out<int> o_copied_arr;

	SC_HAS_PROCESS(Dut);
	Dut(sc_module_name n) : sc_module(n) {
		SC_THREAD(do_compute);
		sensitive << i_clk.pos();
		dont_initialize();
		reset_signal_is(i_rst, false);
	}

	void do_compute() {
		wait();
		while (true) {
			/* === Variable Section === */
			int len = i_arr.size();
			int original_arr[len];
			for (int i = 0; i < len; i++) {
				original_arr[i] = i_arr.read();
			}
			int copied_arr[len];
			for (int i = 0; i < len; i++) {
				copied_arr[i] = original_arr[len - i - 1];
			}
			/* === Variable Section End === */

			/* === Main function Section === */
			printArray(original_arr, len);
			printArray(copied_arr, len);
			/* === Main function Section End === */

			/* === Variable Section === */
			for (int i = 0; i < len; i++) {
				o_copied_arr.write(copied_arr[i]);
			}
			/* === Variable Section End === */
		}
	}
};
