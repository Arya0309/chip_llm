
#include <systemc.h>

SC_MODULE(DivisionModule) {
    sc_in<int> Dividend;     // Input port for Dividend
    sc_in<int> Divisor;      // Input port for Divisor
    sc_out<int> Quotient;    // Output port for Quotient
    sc_out<int> Remainder;  // Output port for Remainder
    sc_out<bool> Error;      // Output port for Error flag

    SC_CTOR(DivisionModule) {
        SC_METHOD(compute);
        sensitive << Dividend << Divisor;
    }

    void compute() {
        if (Divisor.read() == 0) {
            Error.write(true);
            Quotient.write(0);    // Optional: Set Quotient to 0 when error
            Remainder.write(0);   // Optional: Set Remainder to 0 when error
        } else {
            Error.write(false);
            Quotient.write(Dividend.read() / Divisor.read());
            Remainder.write(Dividend.read() % Divisor.read());
        }
    }
};

int sc_main(int argc, char* argv[]) {
    DivisionModule div_mod("DivisionModule");

    sc_signal<int> DividendSig;
    sc_signal<int> DivisorSig;
    sc_signal<int> QuotientSig;
    sc_signal<int> RemainderSig;
    sc_signal<bool> ErrorSig;

    div_mod.Dividend(DividendSig);
    div_mod.Divisor(DivisorSig);
    div_mod.Quotient(QuotientSig);
    div_mod.Remainder(RemainderSig);
    div_mod.Error(ErrorSig);

    // Example input values
    DividendSig.write(20);
    DivisorSig.write(3);

    sc_start();

    // Print the results
    cout << "The Quotient = " << QuotientSig.read() << endl;
    cout << "The Remainder = " << RemainderSig.read() << endl;
    if (ErrorSig.read()) {
        cout << "Error: Divisor cannot be zero." << endl;
    }

    return 0;
}
