/*DO NOT MODIFY THIS FILE*/

#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "SystemPipeline.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

SystemPipeline * sys = NULL;
// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {
	sys = new SystemPipeline("sys");

	sc_start();
	
	delete sys;
	std::cout<< "Simulated time == " << sc_core::sc_time_stamp() << std::endl;

  return 0;
}
