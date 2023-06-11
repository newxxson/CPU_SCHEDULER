#include<stdio.h>
#include "../config/config.h"
#include "FCFS.h"
#include<stdlib.h>

void refresh_current_RR(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, int display){
    if(*ready_queue_start == NULL){
        return;
    }
    else{
        //set waiting start
        (*current_pcb)->waiting_start = cpu_time;
        //make new return_list
        PCB_Q** new_PCB_Q = (PCB_Q**)malloc(sizeof(PCB_Q*));
        (*new_PCB_Q) = (PCB_Q*)malloc(sizeof(PCB_Q));
        (*new_PCB_Q)->pcb = *current_pcb;
        (*new_PCB_Q)->next_elem = NULL;

        add_to_ready_queue_FCFS(new_PCB_Q, ready_queue_start, ready_queue_last);
        
        new_current_FCFS(cpu_time, current_pcb, ready_queue_start, ready_queue_last, display);
        if(display)
            printf("Time Slice all spent. Changing to Process%d\n", (*current_pcb)->process_num);
    }
}

//RR -> sees only arrival time and IO
void RR(PCB** jobs, int total_num, int time_quantum, int display){
    PCB* arr = *jobs;
    //initialize queue
    PCB_Q ** ready_queue_start = (PCB_Q **)malloc(sizeof(PCB_Q*));
    PCB_Q ** ready_queue_last = (PCB_Q **)malloc(sizeof(PCB_Q*));
    //priority queue
    PCB_Q ** wait_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));
    *wait_queue = NULL;
    PCB * current_pcb;

    

    //attaching first pcb -> since the arrival time is zero no wait check needed
    current_pcb = &arr[0];
    current_pcb->response_time = -1;
    *ready_queue_start = NULL;
    *ready_queue_last = NULL;
    
    //since 0 has already come
    int next_income = 1;
    //begin simulating
    int cpu_time = 0;
    int used_time = 0;
    //for breaking while
    int completed_task = 0;
    //start processing
    while(completed_task < total_num && cpu_time < 100){
        //check if process is empty
        if(current_pcb != NULL){
            if(display == 1)
                printf("=====cpu_time:%d, process:%d=====\n",cpu_time, current_pcb->process_num);
            else{
                if(cpu_time%9 == 0)
                    printf("\n");
                printf("cputime%2d: P%d|", cpu_time, current_pcb->process_num);
            }
            //check if any process arrived or returned from IO
            check_process_arrive_FCFS(ready_queue_start, ready_queue_last, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_FCFS(wait_queue, cpu_time, ready_queue_start, ready_queue_last, display);
            if(current_pcb->cpu_burst_used_time == current_pcb->cpu_burst_time){
                current_pcb->turnaround_time = cpu_time - current_pcb->arrival_time;
                completed_task += 1;
                if(display)
                    printf("process: %d completed! with TT:%d, WT:%d, completed process: %d\n", current_pcb->process_num, current_pcb->turnaround_time, current_pcb->total_waiting_time, completed_task);
                if(completed_task == total_num)
                    break;
                new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);
                used_time = 0;
            }
            else{
                //checking if current pcb has IO req
                if(current_pcb->IO_num > 0){
                    //check if IO_req occurred
                    
                    if(current_pcb->IO_req < current_pcb->IO_num && current_pcb->IO_start[current_pcb->IO_req] == current_pcb->cpu_burst_used_time){
                        //add to wait queue
                        add_wait(current_pcb, wait_queue, cpu_time, display);
                        //get new current pcb from next ready_queue
                        new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);
                        used_time = 0;
                    }
                }
            }
            //Round robin part
            if(used_time == time_quantum){
                //put current back in ready queue and get new
                refresh_current_RR(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);
                
                used_time = 0;
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
            check_process_arrive_FCFS(ready_queue_start, ready_queue_last, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_FCFS(wait_queue, cpu_time, ready_queue_start, ready_queue_last, display);
            
            new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);  
            used_time = 0; 
        }
        cpu_time += 1;
        if(current_pcb != NULL){
            used_time += 1;
            current_pcb->cpu_burst_used_time += 1;
        }
    }
    if(display)
        printf("\n\nall process has been completed\n");
    printf("\n\n");
    free_FCFS(ready_queue_start, ready_queue_last, wait_queue);
}