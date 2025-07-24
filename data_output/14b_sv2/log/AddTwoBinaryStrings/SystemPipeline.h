#ifndef SYSTEM_PIPELINE_H_
#define SYSTEM_PIPELINE_H_

#include <systemc>
#include <string>
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

    sc_fifo<std::string> channel_a;
    sc_fifo<std::string> channel_b;
    sc_fifo<std::string> channel_result;
};

#endif // SYSTEM_PIPELINE_H_
