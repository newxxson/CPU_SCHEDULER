#include<stdio.h>
#include "../config/config.h"
#include "PRIORITY.h"
#include<stdlib.h>

void check_preemp_PRIORITY(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue, int display){
    //no ready queue to check from
    if(*ready_queue == NULL){
        return;
    }
    else{
        //check if priority from ready queue > curr
        if((*ready_queue)->pcb->priority > (*current_pcb)->priority){
            
            //setting wait time
            (*current_pcb)->waiting_start = cpu_time;

            //put the process back to ready_queue
            add_to_ready_queue_PRIORITY(ready_queue, *current_pcb);

            //get new current
            *current_pcb = ((*ready_queue)->pcb);

            //check if first time entering cpu
            if((*current_pcb)->total_waiting_time == 0)
                (*current_pcb)->response_time = cpu_time - (*current_pcb)->waiting_start;
            //add waiting time to total_waiting_time
            (*current_pcb)->total_waiting_time += cpu_time - (*current_pcb)->waiting_start;
            //move ready_queue to next and remove elemnet
            PCB_Q* temp = *ready_queue;
            (*ready_queue) = (*ready_queue)->next_elem;
            free(temp);
            if(display)
                printf("changing current with new process: %d\n", (*current_pcb)->process_num);
            return;
        }
        else{   //no preemp needed
            return;
        }
    }
}




void P_PRIORITY(PCB** jobs, int total_num, int display){
    PCB* arr = *jobs;
    //initialize queue
    PCB_Q ** ready_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));
    //priority queue
    PCB_Q ** wait_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));
    *wait_queue = NULL;
    PCB * current_pcb;

    

    //attaching first pcb -> since the arrival time is zero no wait check needed
    int shortest = first_time_check_PRIORITY(arr, total_num);
    current_pcb = &arr[shortest];
    current_pcb->response_time = -1;
    if(display)
        printf("starting with process%d\n", shortest);
    *ready_queue = NULL;
    
    //since 0 has already come
    int next_income = 0;
    //begin simulating
    int cpu_time = 0;
    //for breaking while
    int completed_task = 0;
    //start processing
    int i=0;

    while(completed_task < total_num && cpu_time < 100){
        //check if process is empty
        if(current_pcb != NULL){
            if(display)
                printf("=====cpu_time:%d, process:%d=====\n",cpu_time, current_pcb->process_num);
            else{
                if(cpu_time%9 == 0)
                    printf("\n");
                printf("cputime%2d: P%d|", cpu_time, current_pcb->process_num);
            }
            //check if any process arrived or returned from IO
            check_process_arrive_PRIORITY(ready_queue, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_PRIORITY(wait_queue, cpu_time, ready_queue, display);
            //check if there is need for preemption
            check_preemp_PRIORITY(cpu_time, &current_pcb, ready_queue, display);

            //check if process is complete
            if(current_pcb->cpu_burst_used_time >= current_pcb->cpu_burst_time){
                current_pcb->turnaround_time = cpu_time - current_pcb->arrival_time;
                completed_task += 1;
                if(display)
                    printf("process: %d completed! with TT:%d, WT:%d, completed process: %d\n", current_pcb->process_num, current_pcb->turnaround_time, current_pcb->total_waiting_time, completed_task);
                if(completed_task >= total_num)
                    break;
                new_current_PRIORITY(cpu_time, &current_pcb, ready_queue, display);
            }
            else{
                //checking if current pcb has IO req
                if(current_pcb->IO_num > 0){
                    //check if IO_req occurred
                    
                    if(current_pcb->IO_req < current_pcb->IO_num && current_pcb->IO_start[current_pcb->IO_req] == current_pcb->cpu_burst_used_time){
                        //add to wait queue
                        add_wait(current_pcb, wait_queue, cpu_time, display);
                        //get new current pcb from next ready_queue
                        new_current_PRIORITY(cpu_time, &current_pcb, ready_queue, display);
                    }
                }
            }
        }
        else
        {
            if(display)
                printf("=====cpu_time:%d, process:idle=====\n",cpu_time);
            else{
                if(cpu_time%9 == 0)
                    printf("\n");
                printf("cputime%2d: __|", cpu_time);
            }
            //check if any process arrived or returned from IO
            check_process_arrive_PRIORITY(ready_queue, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_PRIORITY(wait_queue, cpu_time, ready_queue, display);
            
            new_current_PRIORITY(cpu_time, &current_pcb, ready_queue, display);
        }
        cpu_time += 1;
        if(current_pcb != NULL)
            current_pcb->cpu_burst_used_time += 1;
    }
    if(display)
        printf("\n\nall process has been completed\n");
    printf("\n\n");
    free_PRIORITY(ready_queue, wait_queue);
}


