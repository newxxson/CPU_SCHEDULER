#ifndef PRIORITY_H
#define PRIORITY_H

#include<stdio.h>
#include "../config/config.h"
#include<stdlib.h>


void PRIORITY(PCB** jobs, int total_num, int display);
void free_PRIORITY(PCB_Q** ready_queue, PCB_Q** wait_queue);
void add_to_ready_queue_PRIORITY(PCB_Q** ready_queue, PCB* pcb);
int first_time_check_PRIORITY(PCB* arr, int total_num);
int check_time_left(PCB* elem);
void wait_to_ready_PRIORITY(PCB_Q** return_list, PCB_Q** ready_queue);
void check_wait_PRIORITY(PCB_Q** wait_queue, int cpu_time, PCB_Q** ready_queue_start, int display);
void check_process_arrive_PRIORITY(PCB_Q** ready_queue, PCB** jobs, int cpu_time, int* next_income, int total_num, int display);
void new_current_PRIORITY(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue, int display);


#endif