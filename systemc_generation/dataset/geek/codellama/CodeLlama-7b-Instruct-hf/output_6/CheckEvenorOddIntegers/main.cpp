scss
#include <systemc.h>


SC_MODULE(EvenOddChecker)
{
  // Input/output signals
  sc_in<int> number;
  sc_out<string> result;

  void Process()
  {
    int remainder = number % 2;
    if (remainder == 0)
    {
      result = "Even";
    }
    else
    {
      result = "Odd";
    }
  }

  void Run()
  {
    Process();
  }
};

int main()
{
  EvenOddChecker evenodd;
  sc_in<int> number_in;
  sc_out<string> result_out;

  number_in = 11;
  evenodd.Run();
  result_out = evenodd.result;
  cout << "Result: " << result_out.read() << endl;
}


SC_MODULE(Testbench) {
    sc_signal<int> number_sig;   // Signal for the input number
    sc_signal<int> result_sig;   // Signal for the result

    EvenOddChecker checker_inst; // Instance of the EvenOddChecker module

    SC_CTOR(Testbench)
    : checker_inst("checker_inst")
    {
        checker_inst.number_in(number_sig);
        checker_inst.result_out(result_sig);
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case 1: Number 11 (Odd)
        number_sig.write(11);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 1
        assert(result_sig.read() == 1);

        // Test case 2: Number 12 (Even)
        number_sig.write(12);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 0
        assert(result_sig.read() == 0);
        
        std::cout << "All tests pass successfully!" << std::endl;
        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
