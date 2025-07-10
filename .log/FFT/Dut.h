#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct Complex {
    double real;
    double imag;
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    Complex operator+(const Complex &other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    Complex operator-(const Complex &other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    Complex operator*(const Complex &other) const {
        return Complex(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
    }
};

class Dut : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
    sc_fifo_in<Complex> i_a[8];
    sc_fifo_out<Complex> o_result[8];

    SC_HAS_PROCESS(Dut);
    Dut(sc_module_name n);
    ~Dut() = default;

private:
    void do_fft();
};

#endif // DUT_H_