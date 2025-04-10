json
{
  "input_output": {
    "inputs": [
      {
        "name": "number_in",
        "type": "sc_in<int>",
        "description": "The input integer number to be checked for evenness or oddness."
      }
    ],
    "outputs": [
      {
        "name": "result_out",
        "type": "sc_out<int>",
        "description": "The output integer representing whether the number is even (0) or odd (1)."
      }
    ]
  },
  "summary": {
    "line_by_line": [
      {
        "line": "// C++ program to check if the number is even or odd using modulo operator",
        "description": "A comment indicating the purpose of the program."
      },
      {
        "line": "#include <bits/stdc++.h>",
        "description": "Include the standard library header."
      },
      {
        "line": "using namespace std;",
        "description": "Use the standard namespace for convenience."
      },
      {
        "line": "int main() {",
        "description": "Start of the main function."
      },
      {
        "line": "    int n = 11;",
        "description": "Declare and initialize an integer variable 'n' with the value 11."
      },
      {
        "line": "    // If n is completely divisible by 2",
        "description": "A comment explaining the condition for checking if 'n' is even."
      },
      {
        "line": "    if (n % 2 == 0)",
        "description": "Check if 'n' is divisible by 2 without a remainder."
      },
      {
        "line": "        cout << \"Even\";",
        "description": "Output 'Even' to the console if the condition is true."
      },
      {
        "line": "    // If n is NOT completely divisible by 2",
        "description": "A comment explaining the condition for checking if 'n' is odd."
      },
      {
        "line": "    else",
        "description": "Execute the following block if the previous condition is false."
      },
      {
        "line": "        cout << \"Odd\";",
        "description": "Output 'Odd' to the console if the condition is false."
      },
      {
        "line": "    return 0;",
        "description": "Return 0 to indicate successful execution of the program."
      },
      {
        "line": "}",
        "description": "End of the main function."
      }
    ],
    "block_by_block": [
      {
        "block": "int main() { ... }",
        "description": "The main function where the program logic is implemented. It initializes a variable 'n', checks if 'n' is even or odd using the modulo operator, and prints the result to the console."
      }
    ]
  },
  "systemc_code": {
    "code_snippet": [
      {
        "line": "#include <systemc.h>",
        "description": "Include the SystemC library header."
      },
      {
        "line": "
SC_MODULE(EvenOddChecker) {",
        "description": "Define a SystemC module named 'EvenOddChecker'."
      },
      {
        "line": "    sc_in<int> number_in;",
        "description": "Declare an input port 'number_in' to receive the integer input."
      },
      {
        "line": "    sc_out<int> result_out;",
        "description": "Declare an output port 'result_out' to send the result."
      },
      {
        "line": "    SC_CTOR(EvenOddChecker) : number_in(\"number_in\"), result_out(\"result_out\") {",
        "description": "Constructor for the module, initializing the input and output ports."
      },
      {
        "line": "        SC_METHOD(check_even_odd);",
        "description": "Register the 'check_even_odd' method to be executed when the input changes."
      },
      {
        "line": "        sensitive << number_in;",
        "description": "Specify that the method should be sensitive to changes in 'number_in'."
      },
      {
        "line": "    }",
        "description": "End of the constructor."
      },
      {
        "line": "    void check_even_odd() {",
        "description": "Define the 'check_even_odd' method to perform the even-odd check."
      },
      {
        "line": "        int n = number_in.read();",
        "description": "Read the value from the input port 'number_in'."
      },
      {
        "line": "        if (n % 2 == 0)",
        "description": "Check if 'n' is divisible by 2 without a remainder."
      },
      {
        "line": "            result_out.write(0);",
        "description": "Write 0 to 'result_out' if 'n' is even."
      },
      {
        "line": "        else",
        "description": "Execute the following block if the previous condition is false."
      },
      {
        "line": "            result_out.write(1);",
        "description": "Write 1 to 'result_out' if 'n' is odd."
      },
      {
        "line": "    }",
        "description": "End of the 'check_even_odd' method."
      },
      {
        "line": "};",
        "description": "End of the 'EvenOddChecker' module definition."
      }
    ]
  }
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
