#include<stdio.h>
#include "../config/config.h"
#include<stdlib.h>

void free_SJF(PCB_Q** ready_queue, PCB_Q** wait_queue);
void add_to_ready_queue_SJF(PCB_Q** ready_queue, PCB* pcb);
int first_time_check_SJF(PCB* arr, int total_num);
int check_time_left(PCB* elem);

//adds return_list to ready_queue
void wait_to_ready_SJF(PCB_Q** return_list, PCB_Q** ready_queue){
    PCB_Q* temp;
    temp = *return_list;
    while(*return_list != NULL){
        add_to_ready_queue_SJF(ready_queue, ((*return_list)->pcb));
        (*return_list) = (*return_list)->next_elem;
        free(temp);
        temp = (*return_list);
    }
    //free the return_list
    free(return_list);
}

void add_to_ready_queue_SJF(PCB_Q** ready_queue, PCB* pcb){
    //defining return_time and changing request number
    
    PCB_Q * new_elem = (PCB_Q*)malloc(sizeof(PCB_Q));
    new_elem->pcb = pcb;
    new_elem->next_elem = NULL;
    //start ready queue
    if(*ready_queue == NULL){
        *ready_queue = new_elem;
    }
    else{
        PCB_Q * current_elem;
        PCB_Q * previous_elem;
        previous_elem = *ready_queue;
        current_elem = *ready_queue;
        //moving elem back until becomes small
        while(current_elem != NULL && check_time_left(current_elem->pcb) <= check_time_left(new_elem->pcb)){
            previous_elem = current_elem;
            current_elem = current_elem->next_elem;
        }
        //now new elem is smaller

        //if previous and current_elem is at the start
        if(previous_elem == current_elem){
            *ready_queue = new_elem;
            new_elem->next_elem = previous_elem;
        }
        else{
            previous_elem->next_elem = new_elem;
            new_elem->next_elem = current_elem;
        }
    }
    
}

//checks wait_queue for IO return then adds to ready_queue
void check_wait_SJF(PCB_Q** wait_queue, int cpu_time, PCB_Q** ready_queue_start, int display){
    PCB_Q** return_list;
    //check if there is IO return
    return_list = check_wait(wait_queue, cpu_time, display);
    //add to ready_queue, method is FCFS specific
    wait_to_ready_SJF(return_list, ready_queue_start);
}

void check_process_arrive_SJF(PCB_Q** ready_queue, PCB** jobs, int cpu_time, int* next_income, int total_num, int display){
    PCB* arr = *jobs;
    int no_check = total_num;
    if(*next_income >= total_num)
        return;
    if(cpu_time == 0)
        no_check = first_time_check_SJF(arr, total_num);
    while((*next_income < total_num) && ((arr[*next_income].arrival_time) == cpu_time)){
        if(cpu_time == 0 && no_check == *next_income){ //since it is already in current
            *next_income += 1;
            continue;
        }
        if(display)
            printf("process%d arrived\n", arr[*next_income].process_num);
        add_to_ready_queue_SJF(ready_queue, &arr[*next_income]);
        *next_income += 1;
    }
}

void new_current_SJF(int cpu_time, PCB** current_pcb, PCB_Q** ready_queue, int display){
    if(*ready_queue == NULL){
        *current_pcb = NULL;
        return;
    }
    else{
        *current_pcb = ((*ready_queue)->pcb);

        //check if first time entering cpu
        if((*current_pcb)->total_waiting_time == 0 && (*current_pcb)->response_time != -1)
                (*current_pcb)->response_time = cpu_time - (*current_pcb)->waiting_start;
        //add waiting time to total_waiting_time
        (*current_pcb)->total_waiting_time += cpu_time - (*current_pcb)->waiting_start;
        //move ready_queue to next and remove elemnet
        PCB_Q* temp = *ready_queue;
        (*ready_queue) = (*ready_queue)->next_elem;
        free(temp);
        if(display)
            printf("changing to proccess%d\n", (*current_pcb)->process_num);
        // printf("changing new current with process: %d\n", (*current_pcb)->process_num);
    }
}

int first_time_check_SJF(PCB* arr, int total_num){
    int shortest = 0;
    int i=0;

    while(arr[i].arrival_time == 0){
        if(arr[shortest].cpu_burst_time > arr[i].cpu_burst_time){
            shortest = i;
        }
        i++;
    }
    return shortest;
}

int check_time_left(PCB* elem){
    return (elem->cpu_burst_time - elem->cpu_burst_used_time);
}



//FCFS -> sees only arrival time and IO
void SJF(PCB** jobs, int total_num, int display){
    PCB* arr = *jobs;
    //initialize queue
    PCB_Q ** ready_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));
    //priority queue
    PCB_Q ** wait_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));
    *wait_queue = NULL;
    PCB * current_pcb;

    

    //attaching first pcb -> since the arrival time is zero no wait check needed
    
    int shortest = first_time_check_SJF(arr, total_num);
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
        // i++;
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
            check_process_arrive_SJF(ready_queue, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_SJF(wait_queue, cpu_time, ready_queue, display);
            if(current_pcb->cpu_burst_used_time == current_pcb->cpu_burst_time){
                current_pcb->turnaround_time = cpu_time - current_pcb->arrival_time;
                completed_task += 1;
                if(display)
                    printf("process: %d completed! with TT:%d, WT:%d, completed process: %d\n", current_pcb->process_num, current_pcb->turnaround_time, current_pcb->total_waiting_time, completed_task);
                if(completed_task == total_num)
                    break;
                new_current_SJF(cpu_time, &current_pcb, ready_queue, display);
            }
            else{
                //checking if current pcb has IO req
                if(current_pcb->IO_num > 0){
                    //check if IO_req occurred
                    
                    if(current_pcb->IO_req < current_pcb->IO_num && current_pcb->IO_start[current_pcb->IO_req] == current_pcb->cpu_burst_used_time){
                        //add to wait queue
                        add_wait(current_pcb, wait_queue, cpu_time, display);
                        //get new current pcb from next ready_queue
                        new_current_SJF(cpu_time, &current_pcb, ready_queue, display);
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
                printf("cputime%2d: __|", cpu_time);}
            //check if any process arrived or returned from IO
            check_process_arrive_SJF(ready_queue, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_SJF(wait_queue, cpu_time, ready_queue, display);
            
            new_current_SJF(cpu_time, &current_pcb, ready_queue, display);
        }
        cpu_time += 1;
        if(current_pcb != NULL)
            current_pcb->cpu_burst_used_time += 1;
    }
    if(display)
        printf("\n\nall process has been completed\n");
    else
        printf("\n\n");
    free_SJF(ready_queue, wait_queue);
}

void free_SJF(PCB_Q** ready_queue, PCB_Q** wait_queue){
    free(ready_queue);
    free_wait_queue(wait_queue);
}