json
{
  "inputs": [
    {
      "name": "year",
      "type": "int",
      "description": "The year to be checked for leap year status."
    }
  ],
  "outputs": [
    {
      "name": "is_leap_year",
      "type": "bool",
      "description": "Indicates whether the given year is a leap year or not."
    }
  ],
  "summary": {
    "line-by-line": [
      {
        "line": "// C++ program to check if a given year is a leap year or not",
        "description": "Comment indicating the purpose of the program."
      },
      {
        "line": "#include <iostream>",
        "description": "Include the iostream library for input and output operations."
      },
      {
        "line": "using namespace std;",
        "description": "Use the standard namespace to avoid prefixing standard library names with 'std::'."
      },
      {
        "line": "// Function to check leap year",
        "description": "Comment indicating the start of the function definition."
      },
      {
        "line": "bool checkYear(int year)",
        "description": "Define a function named 'checkYear' that takes an integer 'year' as input and returns a boolean value."
      },
      {
        "line": "{",
        "description": "Start of the function body."
      },
      {
        "line": "    if (year % 400 == 0) {",
        "description": "Check if the year is divisible by 400. If true, it is a leap year."
      },
      {
        "line": "        return true;",
        "description": "Return true if the year is divisible by 400."
      },
      {
        "line": "    }",
        "description": "End of the first condition block."
      },
      {
        "line": "    // not a leap year if divisible by 100 but not divisible by 400",
        "description": "Comment explaining the next condition."
      },
      {
        "line": "    else if (year % 100 == 0) {",
        "description": "Check if the year is divisible by 100 but not by 400. If true, it is not a leap year."
      },
      {
        "line": "        return false;",
        "description": "Return false if the year is divisible by 100 but not by 400."
      },
      {
        "line": "    }",
        "description": "End of the second condition block."
      },
      {
        "line": "    // leap year if not divisible by 100 but divisible by 4",
        "description": "Comment explaining the next condition."
      },
      {
        "line": "    else if (year % 4 == 0) {",
        "description": "Check if the year is divisible by 4 but not by 100. If true, it is a leap year."
      },
      {
        "line": "        return true;",
        "description": "Return true if the year is divisible by 4 but not by 100."
      },
      {
        "line": "    }",
        "description": "End of the third condition block."
      },
      {
        "line": "    // all other years are not leap years",
        "description": "Comment explaining the final condition."
      },
      {
        "line": "    else {",
        "description": "Handle all other cases where the year does not meet any of the above conditions."
      },
      {
        "line": "        return false;",
        "description": "Return false for all other years."
      },
      {
        "line": "    }",
        "description": "End of the fourth condition block."
      },
      {
        "line": "}",
        "description": "End of the function body."
      },
      {
        "line": "// Driver code",
        "description": "Comment indicating the start of the main function."
      },
      {
        "line": "int main()",
        "description": "Define the main function which serves as the entry point of the program."
      },
      {
        "line": "{",
        "description": "Start of the main function body."
      },
      {
        "line": "    int year = 2000;",
        "description": "Declare an integer variable 'year' and initialize it with the value 2000."
      },
      {
        "line": "    checkYear(year) ? cout << \"Leap Year\" : cout << \"Not a Leap Year\";",
        "description": "Call the 'checkYear' function with the 'year' variable as input and print \"Leap Year\" if the function returns true, otherwise print \"Not a Leap Year\"."
      },
      {
        "line": "    return 0;",
        "description": "Return 0 to indicate successful execution of the program."
      },
      {
        "line": "}",
        "description": "End of the main function body."
      }
    ]
  },
  "systemc_code": {
    "code_snippet": [
      "#include <systemc.h>",
      "",
      "
SC_MODULE(LeapYearChecker) {",
      "    sc_in<int> year;",
      "    sc_out<bool> is_leap_year;",
      "",
      "    SC_CTOR(LeapYearChecker) {",
      "        SC_METHOD(checkLeapYear);",
      "        sensitive << year;",
      "    }",
      "",
      "    void checkLeapYear() {",
      "        if (year.read() % 400 == 0) {",
      "            is_leap_year.write(true);",
      "        } else if (year.read() % 100 == 0) {",
      "            is_leap_year.write(false);",
      "        } else if (year.read() % 4 == 0) {",
      "            is_leap_year.write(true);",
      "        } else {",
      "            is_leap_year.write(false);",
      "        }",
      "    }",
      "};",
      "",
      "

SC_MODULE(Testbench) {
    sc_signal<int> year;             // Signal for the year
    sc_signal<bool> is_leap_year;    // Signal for the leap year check result

    LeapYearChecker checker_inst;    // Instance of the LeapYearChecker module

    // Constructor
    SC_CTOR(Testbench) : checker_inst("checker") {
        // Connect signals to ports
        checker_inst.year(year);
        checker_inst.is_leap_year(is_leap_year);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Year 2000 (Leap Year)
        year = 2000;
        wait(1, SC_NS);
        assert(is_leap_year.read() == true);
        cout << "Year 2000: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 2: Year 1900 (Not a Leap Year)
        year = 1900;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 1900: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 3: Year 2004 (Leap Year)
        year = 2004;
        wait(1, SC_NS);
        assert(is_leap_year.read() == true);
        cout << "Year 2004: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Test case 4: Year 2001 (Not a Leap Year)
        year = 2001;
        wait(1, SC_NS);
        assert(is_leap_year.read() == false);
        cout << "Year 2001: " << (is_leap_year.read() ? "Leap Year" : "Not a Leap Year") << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
