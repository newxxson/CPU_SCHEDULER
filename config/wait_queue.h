#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H
#include"PCB.h"
void add_wait(PCB* pcb, PCB_Q** wait_queue, int cpu_time, int display);
PCB_Q** check_wait(PCB_Q** wait_queue, int cpu_time, int display);
void free_wait_elem_recursive(PCB_Q* wait_elem);
void free_wait_queue(PCB_Q** wait_queue);

#endif