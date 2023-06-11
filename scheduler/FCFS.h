#ifndef FCFS_H
#define FCFS_H

#include "../config/config.h"
#include<stdlib.h>

//adds return_list to ready_queue
void add_to_ready_queue_FCFS(PCB_Q** return_list, PCB_Q** ready_queue_start, PCB_Q**ready_queue_last);
//checks wait_queue for IO return then adds to ready_queue
void check_wait_FCFS(PCB_Q** wait_queue, int cpu_time, PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, int display);
void check_process_arrive_FCFS(PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, PCB** arr, int cpu_time, int* next_income, int total_num, int display);
void new_current_FCFS(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, int display);
void FCFS(PCB** jobs, int total_num, int display);
void free_FCFS(PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, PCB_Q** wait_queue);

#endif