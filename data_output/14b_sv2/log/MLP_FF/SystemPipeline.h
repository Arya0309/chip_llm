#ifndef SYSTEM_PIPELINE_H_
#define SYSTEM_PIPELINE_H_

#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#include "Dut.h"

class SystemPipeline : public sc_module {
public:
  SC_HAS_PROCESS(SystemPipeline);
  SystemPipeline(sc_module_name n);
  ~SystemPipeline() = default;

private:
  Testbench tb;
  Dut       dut;

  sc_clock  clk;
  sc_signal<bool> rst;

  sc_fifo<float> channel_input;
  sc_fifo<float> channel_w1;
  sc_fifo<float> channel_b1;
  sc_fifo<float> channel_w2;
  sc_fifo<float> channel_b2;
  sc_fifo<float> channel_output;
};

#endif // SYSTEM_PIPELINE_H_
