#include<stdio.h>
#include"PCB.h"
#include<stdlib.h>
//add PCB to wait_queue
void add_wait(PCB* pcb, PCB_Q** wait_queue, int cpu_time, int display){
    //defining return_time and changing request number
    pcb->IO_return_time =  cpu_time + (pcb->IO_burst_time)[(pcb->IO_req)];
    pcb->IO_req += 1;
    //changin wait_start to IO return time
    pcb->waiting_start = pcb->IO_return_time;
    
    PCB_Q * new_elem = (PCB_Q*)malloc(sizeof(PCB_Q));
    new_elem->pcb = pcb;
    new_elem->next_elem = NULL;
    //start wait queue
    if(*wait_queue == NULL){
        *wait_queue = new_elem;
        if(display)
            printf("IO_request occurred with process: %d, IO_return at: %d\n", pcb->process_num, pcb->IO_return_time);
    }
    else{
        PCB_Q * current_elem;
        PCB_Q * previous_elem;
        previous_elem = *wait_queue;
        current_elem = *wait_queue;
        //moving elem back until becomes small
        while(current_elem != NULL && (current_elem->pcb->IO_return_time) <= (new_elem->pcb->IO_return_time)){
            previous_elem = current_elem;
            current_elem = current_elem->next_elem;
        }
        //now new elem is smaller

        //if previous and current_elem is at the start
        if(previous_elem == current_elem){
            *wait_queue = new_elem;
            new_elem->next_elem = previous_elem;
        }
        else{
            previous_elem->next_elem = new_elem;
            new_elem->next_elem = current_elem;
        }
        if(display)
            printf("IO_request occurred with process: %d, IO_return at: %d\n", pcb->process_num, pcb->IO_return_time);
    }
    
}

//checks if any PCB has finished IO req, returning PCB_Q
//retrieving each PCB is implemented in each algorithm file
PCB_Q** check_wait(PCB_Q** wait_queue, int cpu_time, int display){
    PCB_Q* current_elem = *wait_queue;

    PCB_Q** return_list = (PCB_Q**)malloc(sizeof(PCB_Q*));
    *return_list = NULL;
    PCB_Q* return_list_current = NULL;


    while(current_elem != NULL && current_elem->pcb->IO_return_time == cpu_time){
        if(display)
            printf("return from IO wait with process: %d\n", current_elem->pcb->process_num);
        if(*return_list == NULL){ //empty
            //need to add PCB_Q to return_list from wait_queue
            *return_list = current_elem;
            //pop wait_queue front
            *wait_queue = current_elem->next_elem;
            current_elem->next_elem = NULL;
            return_list_current = *return_list;
            current_elem = *wait_queue;
        }//return_list not empty
        else{//add to PCB_Q next_elem remove from wait_queue
            return_list_current->next_elem = current_elem;
            return_list_current = return_list_current->next_elem;
            
            *wait_queue = current_elem->next_elem;
            current_elem->next_elem = NULL;
            current_elem = *wait_queue;
        }
    }
    return return_list;
}


//removes only the PCB_Q within the wait_queue
//the PCB within is intact
void free_wait_elem_recursive(PCB_Q* wait_elem);

void free_wait_queue(PCB_Q** wait_queue){
    
    PCB_Q* current_elem = *wait_queue;

    free_wait_elem_recursive(current_elem);

    free(wait_queue);
}
//recursively removes all the queue PCB element
void free_wait_elem_recursive(PCB_Q* wait_elem){
    if(wait_elem == NULL){
        return;
    }
    if(wait_elem->next_elem != NULL)
        free_wait_elem_recursive(wait_elem->next_elem);
    
    free(wait_elem);
}