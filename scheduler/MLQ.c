#include<stdio.h>
#include "../config/config.h"
#include<stdlib.h>
#include "scheduler.h"

float get_cpu_burst_avg(PCB arr){
    float cpu_avg = 0;
    if(arr.IO_num == 0){
        return arr.cpu_burst_time;
    }
    else{//has IO
        for(int i=0;i<=arr.IO_num;i++){
            if(i==0)
                cpu_avg += arr.IO_start[i];
            else if(i<arr.IO_num)   //add gap between each IO req
                cpu_avg += arr.IO_start[i] - arr.IO_start[i-1];
            else
                cpu_avg += arr.cpu_burst_time - arr.IO_start[i-1];
        }
        return cpu_avg / (arr.IO_num + 1);
    }
}

void check_cpu_avg(PCB** jobs, int total_num, float criterion){
    PCB* arr = *jobs;
    float cpu_burst_avg = 0;
    for(int i=0;i<total_num;i++){
        cpu_burst_avg = get_cpu_burst_avg(arr[i]);
        //if cpu_burst_avg > criterion -> long_process -> priority = 0
        if(cpu_burst_avg > criterion){
            arr[i].priority = 0;
        }
        else{   //short process
            arr[i].priority = 1;
        }
    }
}   

RQ_ZIP* make_RQ(){
    RQ_ZIP* RQ =  (RQ_ZIP*)malloc(sizeof(RQ_ZIP));
    RQ->fore_start = (PCB_Q**)malloc(sizeof(PCB_Q));
    RQ->fore_last = (PCB_Q**)malloc(sizeof(PCB_Q));
    RQ->back_start = (PCB_Q**)malloc(sizeof(PCB_Q));
    RQ->back_start = (PCB_Q**)malloc(sizeof(PCB_Q));
    *(RQ->fore_start) = NULL;
    *(RQ->fore_last) = NULL;
    *(RQ->back_start) = NULL;
    *(RQ->back_start) = NULL;

    return RQ;
}

PCB* get_first_process(PCB*arr, int total_num, int display){
    int i=0;
    int short_process = 0;

    //process that arrives at 0 and is short if not just process 0
    while(i < total_num && arr[i].arrival_time == 0){
        if(arr[i].priority == 1){
            short_process = i;
            break;
        }
        i++;
    }
    if(display && arr[short_process].priority == 1)
        printf("starting with process %d from fore with avg:%.2f\n", arr[short_process].process_num, get_cpu_burst_avg(arr[short_process]));
    else if(display && arr[short_process].priority == 0){
        printf("starting with process%d from back with avg:%.2f\n", arr[short_process].process_num, get_cpu_burst_avg(arr[short_process]));
    }
    return &arr[short_process];
}

void divide_to_ready_MLQ(RQ_ZIP* RQ, PCB_Q* elem){
    //check long short
    if(elem->pcb->priority == 1){
        if(*(RQ->fore_start) == NULL){ //queue empty
            *(RQ->fore_start) = elem;
            *(RQ->fore_last) = elem;
        }
        else{
            (*(RQ->fore_last))->next_elem = elem;
            *(RQ->fore_last) = (*(RQ->fore_last))->next_elem;
        }
    }
    else{
        if(*(RQ->back_start) == NULL){ //queue empty
            *(RQ->back_start) = elem;
            *(RQ->back_last) = elem;
        }
        else{
            (*(RQ->back_last))->next_elem = elem;
            *(RQ->back_last) = (*(RQ->back_last))->next_elem;
        }
    }
}


void check_process_arrive_MLQ(RQ_ZIP* RQ, PCB** jobs, int cpu_time, int* next_income, int total_num, int display){
    PCB* arr = *jobs;
    if(*next_income >= total_num)
        return;
    while((*next_income < total_num) && ((arr[*next_income].arrival_time) == cpu_time)){

        if(arr[*next_income].priority == 1){
            if(display){
                printf("process%d arrived to fore\n", arr[*next_income].process_num);
                printf("process%d avg: %.2f\n", arr[*next_income].process_num, get_cpu_burst_avg(arr[*next_income]));    
            }
        }
        else{
            if(display){
                printf("process%d arrived to back\n", arr[*next_income].process_num);
                printf("process%d avg: %.2f\n", arr[*next_income].process_num, get_cpu_burst_avg(arr[*next_income]));
            }    
        }
        PCB_Q* new_PCB_Q = (PCB_Q*)malloc(sizeof(PCB_Q));
        (new_PCB_Q->pcb) = &arr[*next_income];
        new_PCB_Q->next_elem = NULL;
        *next_income += 1;
        //add depending on length
        divide_to_ready_MLQ(RQ, new_PCB_Q);
    }
}

//adds return_list to ready_queue
void add_to_ready_queue_MLQ(PCB_Q** return_list, RQ_ZIP* RQ){
    PCB_Q* current_elem;
    current_elem = *return_list;
    PCB_Q* temp;
    //no wait return
    if(*return_list == NULL){
        return;
    }
    //ready_queue_empty

    while(current_elem != NULL){
        //short
        divide_to_ready_MLQ(RQ, current_elem);
        current_elem = current_elem->next_elem;
    }
    //free the return_list
    free(return_list);
}

//checks wait_queue for IO return then adds to ready_queue
void check_wait_MLQ(PCB_Q** wait_queue, int cpu_time, RQ_ZIP* RQ, int display){
    PCB_Q** return_list;
    //check if there is IO return
    return_list = check_wait(wait_queue, cpu_time, display);
    //add to ready_queue, different RQ depending on length(priority)
    add_to_ready_queue_MLQ(return_list, RQ);
}

PCB* get_new_from(RQ_ZIP* RQ, int priority, int display){
    PCB_Q* temp;
    PCB* new_current;
    if(priority==1){

        //the fore queue is empty
        if(*(RQ->fore_start) == NULL){

            //check for back
            if(*(RQ->back_start) != NULL){
                if(display)
                    printf("fore queue empty getting from back\n");

                return get_new_from(RQ, 0, display);
            }
            else{
                return NULL;
            }
        }
        else{
            temp = *(RQ->fore_start);
            new_current = temp->pcb;
            //the queue is now empty
            if(*(RQ->fore_start) == *(RQ->fore_last)){
                *(RQ->fore_start) = NULL;
            }
            else{   //not empty
                *(RQ->fore_start) = (*(RQ->fore_start))->next_elem;
            }
            free(temp);
            return new_current;
        }
    }
    else{ //back
        //the back queue is empty
        if(*(RQ->back_start) == NULL){
            //check for back
            if(*(RQ->fore_start) != NULL){
                if(display)
                    printf("back queue empty getting from front\n");

                return get_new_from(RQ, 1, display);
                
            }
            else
                return NULL;
        }
        else{
            temp = *(RQ->back_start);
            new_current = temp->pcb;
            //the queue is now empty
            if(*(RQ->back_start) == *(RQ->back_last)){
                *(RQ->back_start) = NULL;
            }
            else{   //not empty
                *(RQ->back_start) = (*(RQ->back_start))->next_elem;
            }
            free(temp);
            return new_current;
        }
    }
}

void new_current_MLQ(int cpu_time, int ground_used_time, PCB** current_pcb, RQ_ZIP* RQ, int display){
    //check if time for fore or back
    if(ground_used_time < 4){//fore
        *current_pcb = get_new_from(RQ, 1, display); //1 is for short
        //check if first time entering cpu
        if(*current_pcb != NULL){
            if((*current_pcb)->total_waiting_time == 0 && (*current_pcb)->response_time != -1)
                (*current_pcb)->response_time = cpu_time - (*current_pcb)->waiting_start;
            //add waiting time to total_waiting_time
            (*current_pcb)->total_waiting_time += cpu_time - (*current_pcb)->waiting_start;
            if(display)
                printf("changing to proccess%d\n", (*current_pcb)->process_num);
        }
    }
    else{   //back
        *current_pcb = get_new_from(RQ, 0, display);
        if(*current_pcb != NULL){
            if((*current_pcb)->total_waiting_time == 0 && (*current_pcb)->response_time != -1)
                (*current_pcb)->response_time = cpu_time - (*current_pcb)->waiting_start;
            //add waiting time to total_waiting_time
            (*current_pcb)->total_waiting_time += cpu_time - (*current_pcb)->waiting_start;
            if(display)
                printf("changing to proccess%d\n", (*current_pcb)->process_num);
        }
    }
}

void refresh_current_MLQ(int cpu_time, int ground_used_time, PCB** current_pcb, RQ_ZIP* RQ, int display){
    if(ground_used_time < 4){//fore
        //check if fore ready is empty
        if(*(RQ->fore_start) == NULL)
            return;
        //add current back to ready queue
        (*current_pcb)->waiting_start = cpu_time;
        PCB_Q* new_PCB_Q = (PCB_Q*)malloc(sizeof(PCB_Q));
        new_PCB_Q->pcb = *current_pcb;
        divide_to_ready_MLQ(RQ, new_PCB_Q);

        //get new
        *current_pcb = get_new_from(RQ, 1, display);
        if(display)
            printf("Time Slice all spent. Changing to foreground Process%d\n", (*current_pcb)->process_num);
    }
    else{ //back
        //check if fore ready is empty
        if(*(RQ->back_start) == NULL)
            return;
        //add current back to ready queue
        (*current_pcb)->waiting_start = cpu_time;
        PCB_Q* new_PCB_Q = (PCB_Q*)malloc(sizeof(PCB_Q));
        new_PCB_Q->pcb = *current_pcb;
        divide_to_ready_MLQ(RQ, new_PCB_Q);

        //get new
        *current_pcb = get_new_from(RQ, 0, display);
        if(display)
            printf("Time Slice all spent. Changing to background Process%d\n", (*current_pcb)->process_num);
    }
}

//new two ready queue for fore and back
void MLQ(PCB** jobs, int total_num, int criterion, int time_quantum, int display){
    PCB* arr = *jobs;
    //divide process into two parts using cpu_avg
    check_cpu_avg(&arr, total_num, criterion);

    //fore process is RR & back is FCFS both * null
    RQ_ZIP* RQ = make_RQ();
    RQ->fore_start = (PCB_Q**)malloc(sizeof(PCB_Q*));
    RQ->fore_last = (PCB_Q**)malloc(sizeof(PCB_Q*));
    RQ->back_start = (PCB_Q**)malloc(sizeof(PCB_Q*));
    RQ->back_last = (PCB_Q**)malloc(sizeof(PCB_Q*));
    *(RQ->fore_start) = NULL;
    *(RQ->fore_last) = NULL;
    *(RQ->back_start) = NULL;
    *(RQ->back_last) = NULL;
    //priority queue
    PCB_Q ** wait_queue = (PCB_Q **)malloc(sizeof(PCB_Q*));

    PCB * current_pcb;
    
    *wait_queue = NULL;


    current_pcb = get_first_process(arr,total_num, display);

    

    current_pcb->response_time = -1;
    //since 0 has already come
    int next_income = 1;
    //begin simulating
    int cpu_time = 0;
    int used_time = 0;
    //if ground_used_time reaches 4 process change to background for once.
    int ground_used_time = 0;
    
    //for breaking while
    int completed_task = 0;
    printf("\n");

    while(completed_task < total_num && cpu_time < 100){
        //check if process is empty
        if(current_pcb != NULL){
            if(display)
                printf("=====cpu_time:%d, process:%d, ground:%s=====\n",cpu_time, current_pcb->process_num, current_pcb->priority?"fore":"back");
            else{
                if(cpu_time%9 == 0)
                    printf("\n");
                printf("cputime%2d: P%d|", cpu_time, current_pcb->process_num);
            }
            //check if any process arrived or returned from wait and sort by fore, back
            check_process_arrive_MLQ(RQ, &arr, cpu_time, &next_income, total_num, display);
            check_wait_MLQ(wait_queue, cpu_time, RQ, display);
            //process complete
            if(current_pcb->cpu_burst_used_time >= current_pcb->cpu_burst_time){
                current_pcb->turnaround_time = cpu_time - current_pcb->arrival_time;
                completed_task += 1;
                if(display)
                    printf("process: %d completed! with TT:%d, WT:%d, completed process: %d\n", current_pcb->process_num, current_pcb->turnaround_time, current_pcb->total_waiting_time, completed_task);
                if(completed_task >= total_num)
                    break;
                used_time = 0;
                ground_used_time = (ground_used_time + 1) % 5;
                new_current_MLQ(cpu_time, ground_used_time, &current_pcb, RQ, display);
                if(current_pcb != NULL && current_pcb->priority == 0)
                    ground_used_time = 0;
            }
            else{
                //check if current pcb has IO req
                if(current_pcb->IO_num > 0){
                    //check IO
                    if(current_pcb->IO_req < current_pcb->IO_num && current_pcb->IO_start[current_pcb->IO_req] == current_pcb->cpu_burst_used_time){
                        //add to wait queue
                        add_wait(current_pcb, wait_queue, cpu_time, display);
                        //get new current pcb from next RQ
                        used_time = 0;
                        if(current_pcb->priority == 1)
                            ground_used_time = (ground_used_time + 1) % 5;
                        else
                            ground_used_time = 0;
                        new_current_MLQ(cpu_time, ground_used_time, &current_pcb, RQ, display);
                        if(current_pcb != NULL)
                            if(current_pcb->priority == 0)
                                ground_used_time = 0;
                    }
                }
            }
            //check round robin
            if(used_time == time_quantum){
                if(current_pcb != NULL && current_pcb->priority == 1)
                    ground_used_time = (ground_used_time + 1) % 5;
                else if(current_pcb != NULL && current_pcb->priority == 0)
                    ground_used_time = 0;
                used_time = 0;
                if(current_pcb != NULL)
                    refresh_current_MLQ(cpu_time, ground_used_time, &current_pcb, RQ, display);
                if(current_pcb != NULL && current_pcb->priority == 0)
                    ground_used_time = 0;
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
            check_process_arrive_MLQ(RQ, &arr, cpu_time, &next_income, total_num, display);    
            check_wait_MLQ(wait_queue, cpu_time, RQ, display);
            
            new_current_MLQ(cpu_time, ground_used_time, &current_pcb, RQ, display);  
            used_time = 0;

            if(current_pcb != NULL && current_pcb->priority == 0)
                ground_used_time = 0;
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
    free(RQ->fore_start);
    free(RQ->fore_last);
    free(RQ->back_start);
    free(RQ->back_last);
    free(RQ);
    free(wait_queue);

}

