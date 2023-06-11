#include<stdio.h>
#include "../config/config.h"
#include<stdlib.h>

void free_FCFS(PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, PCB_Q** wait_queue);

//adds return_list to ready_queue
void add_to_ready_queue_FCFS(PCB_Q** return_list, PCB_Q** ready_queue_start, PCB_Q**ready_queue_last){
    PCB_Q* current_elem;
    current_elem = *return_list;
    if(*return_list == NULL){
        return;
    }
    //ready_queue_empty
    if((*ready_queue_start) == NULL){
        //add the return_list chain to ready_queue_start
        (*ready_queue_start) = current_elem;

        //moving ready_queue_last to last
        while(current_elem->next_elem != NULL){
    
            current_elem = current_elem->next_elem;
        }
        *ready_queue_last = current_elem;
    }
    else{
        //just add to ready_queue last since fcfs is fcfs
        (*ready_queue_last)->next_elem = current_elem;

        //moving ready_queue_last to last
        while(current_elem->next_elem != NULL){
    
            current_elem = current_elem->next_elem;
        }
        *ready_queue_last = current_elem;
    }

    //free the return_list
    free(return_list);
}

//checks wait_queue for IO return then adds to ready_queue
void check_wait_FCFS(PCB_Q** wait_queue, int cpu_time, PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, int display){
    PCB_Q** return_list;
    //check if there is IO return
    return_list = check_wait(wait_queue, cpu_time, display);
    //add to ready_queue, method is FCFS specific
    add_to_ready_queue_FCFS(return_list, ready_queue_start, ready_queue_last);
}

void check_process_arrive_FCFS(PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, PCB** jobs, int cpu_time, int* next_income, int total_num, int display){
    PCB* arr = *jobs;
    if(*next_income >= total_num)
        return;
    while((*next_income < total_num) && ((arr[*next_income].arrival_time) == cpu_time)){
        
        if(display)
            printf("process%d arrived\n", arr[*next_income].process_num);
        PCB_Q* new_PCB_Q = (PCB_Q*)malloc(sizeof(PCB_Q));
        (new_PCB_Q->pcb) = &arr[*next_income];
        new_PCB_Q->next_elem = NULL;
        *next_income += 1;
        //check if ready_queue empty
        if(*ready_queue_start == NULL){
            *ready_queue_start = new_PCB_Q;
            *ready_queue_last = new_PCB_Q;
        }
        else{
            (*ready_queue_last)->next_elem = new_PCB_Q;
            (*ready_queue_last) = (*ready_queue_last)->next_elem;
        }
    }
}

void new_current_FCFS(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, int display){
    if(*ready_queue_start == NULL){
        *current_pcb = NULL;
        return;
    }
    else{
        *current_pcb = ((*ready_queue_start)->pcb);
        if(display)
            printf("changing to proccess%d\n", (*current_pcb)->process_num);
        //check if first time entering cpu
        if((*current_pcb)->total_waiting_time == 0 && (*current_pcb)->response_time != -1)
            (*current_pcb)->response_time = cpu_time - (*current_pcb)->waiting_start;
        //add waiting time to total_waiting_time
        (*current_pcb)->total_waiting_time += cpu_time - (*current_pcb)->waiting_start;
        //move ready_queue_start to next and remove elemnet
        PCB_Q* temp = *ready_queue_start;
        (*ready_queue_start) = (*ready_queue_start)->next_elem;
        free(temp);

        if(*ready_queue_start == NULL)//the queue is empty -> idle
            *ready_queue_last = NULL;
        // printf("changing new current with process: %d\n", (*current_pcb)->process_num);
    }
}


//FCFS -> sees only arrival time and IO
void FCFS(PCB** jobs, int total_num, int display){
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
                    printf("process: %d completed! with TT:%d, WT:%d, completed process: %d\n", current_pcb->process_num, 
                    current_pcb->turnaround_time, current_pcb->total_waiting_time, completed_task);
                if(completed_task == total_num)
                    break;
                new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);
            }
            else{
                //checking if current pcb has IO req
                if(current_pcb->IO_num > 0){
                    //check if IO_req occurred
                    if(current_pcb->IO_req < current_pcb->IO_num && 
                    current_pcb->IO_start[current_pcb->IO_req] == current_pcb->cpu_burst_used_time){
                        //add to wait queue
                        add_wait(current_pcb, wait_queue, cpu_time, display);
                        //get new current pcb from next ready_queue
                        new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);
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
            check_process_arrive_FCFS(ready_queue_start, ready_queue_last, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_FCFS(wait_queue, cpu_time, ready_queue_start, ready_queue_last, display);
            
            new_current_FCFS(cpu_time, &current_pcb, ready_queue_start, ready_queue_last, display);   
        }
        cpu_time += 1;
        if(current_pcb != NULL)
            current_pcb->cpu_burst_used_time += 1;
    }
    if(display)
        printf("\n\nall process has been completed\n");
    printf("\n\n");
    free_FCFS(ready_queue_start, ready_queue_last, wait_queue);
}

void free_FCFS(PCB_Q** ready_queue_start, PCB_Q** ready_queue_last, PCB_Q** wait_queue){
    free(ready_queue_start);
    free(ready_queue_last);
    free_wait_queue(wait_queue);
}