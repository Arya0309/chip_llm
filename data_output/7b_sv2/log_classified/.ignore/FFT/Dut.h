
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

const double PI = 3.14159265358979323846;

double sin_approx(double x);
double cos_approx(double x);
void fft(Complex *a, int n);

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  /* === Variable Section === */
  sc_fifo_in<double> i_a_real;
  sc_fifo_in<double> i_a_imag;
  sc_fifo_out<double> o_result_real;
  sc_fifo_out<double> o_result_imag;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_
