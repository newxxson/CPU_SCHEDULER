#ifndef PCB_H
#define PCB_H



#include<stdbool.h>

typedef struct PCB_Q PCB_Q;
typedef struct RQ_ZIP RQ_ZIP;

typedef struct PCB
{
    //don't know why needed
    int pid;
    int process_num;
    //processes arrival time
    int arrival_time;
    //total cpu_burst_time
    int cpu_burst_used_time;
    int cpu_burst_time;

    //number of IO reqs, current_req, when it starts, how long, total time, when it returns
    int IO_num;
    int IO_req;
    int * IO_start;
    int * IO_burst_time;
    int total_IO_burst_time;
    int IO_return_time;

    int priority;

    //for analysis purpose
    int total_waiting_time;
    int waiting_start;
    int turnaround_time;
    int response_time;

} PCB;

struct PCB_Q
{
    PCB * pcb;
    PCB_Q * next_elem;
};

struct RQ_ZIP
{
    PCB_Q** fore_start;
    PCB_Q** fore_last;
    PCB_Q** back_start;
    PCB_Q** back_last;
};





#endif