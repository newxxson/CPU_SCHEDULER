#ifndef INITIALIZE_H
#define INITIALIZE_H

#include "PCB.h"
int * distinct_random_values(int total_num);
int * create_IO_start(int cpu_burst_time, int IO_num);
int * create_IO_burst(int IO_num, int IO_limit, int * total_IO_burst_time);
PCB* Create_Process(int total_num, int cpu_limit, int IO_limit, int IO_time_limit, int start_at_0);
void free_PCB(PCB* arr, int total_num);
void reset_PCB(PCB* arr, int total_num);
#endif