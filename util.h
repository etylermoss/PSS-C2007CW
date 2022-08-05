#ifndef __UTIL_H__
#define __UTIL_H__

enum SimulatorProgram
{
	SIM_1,
	SIM_2,
	SIM_3,
	SIM_4,
	SIM_5,
	SIM_6,
	SIM_7,
};

void fatal_error(const char* str);
void fatal_errorf(const char* fmt, ...);

#endif