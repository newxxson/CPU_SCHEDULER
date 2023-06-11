#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "PCB.h"
#include<stdbool.h>

//input is a array of PCB and a total number written by user
//creates array of PCBs based on random.


int * create_IO_start(int cpu_burst_time, int IO_num){
    
    int * IO_start = (int *)malloc(sizeof(int)*IO_num);

    for(int k=0;k<IO_num;k++){    
        if(k==0)//can't be larger than cpu time - number of io invoke
            IO_start[k] = rand() % (cpu_burst_time - (IO_num - 1) - 1) + 1;
        else if(k>0)//has to be larger than the before IO start but smaller than the cpu time - remaining number
            IO_start[k] = (rand() % (cpu_burst_time - (IO_num - 1 - k) - IO_start[k-1] -1) + 1)  + IO_start[k-1] ;
    }

    return IO_start;
}

int * create_IO_burst(int IO_num, int IO_time_limit, int * total_IO_burst_time){
    int * IO_burst_time = (int *)malloc(sizeof(int)*IO_num);
    int temp;
    for(int i=0;i<IO_num;i++){
        temp = rand() % IO_time_limit + 1;
        IO_burst_time[i] = temp;
        *total_IO_burst_time += temp;
    }

    return IO_burst_time;
}

int * distinct_random_values(int total_num) {
    int *array = malloc(sizeof(int) * total_num);
    int i, j, rand_value;


    for (i = 0; i < total_num; i++) {
        rand_value = rand() % total_num + 1;
        for (j = 0; j < i; j++) {
            if (array[j] == rand_value) {
                rand_value = rand() % total_num + 1;
                j = -1;
            }
        }
        array[i] = rand_value;
    }

    return array;
}



//initializes PCBs with random value
//input: total number of PCB, their max cpu burst length, max IO time, bool for if for all processes start at 0
PCB* Create_Process(int total_num, int cpu_limit, int IO_limit, int IO_time_limit, int start_at_0){

    srand((int)time(NULL));

    int * random_priority = distinct_random_values(total_num);



    PCB * arr;
    arr = (PCB *)malloc(sizeof(PCB) * total_num);
    for(int i=0;i<total_num;i++){
        arr[i].pid = rand() % 1000  + 1;
        arr[i].process_num = i;
        arr[i].cpu_burst_time = rand() % cpu_limit + 1;
        arr[i].cpu_burst_used_time = 0;

        //IO start time
        do{
            arr[i].IO_num = rand() % (IO_limit + 1);
        }while(arr[i].IO_num >= arr[i].cpu_burst_time); //up to IO_limit times but can't be larger than cpu_burst_time itself;
        if(arr[i].IO_num == 0)
            arr[i].IO_start = NULL;
        else{
            if(arr[i].IO_num >= arr[i].cpu_burst_time)
                arr[i].IO_num = arr[i].cpu_burst_time -1;
            arr[i].IO_start = create_IO_start(arr[i].cpu_burst_time, arr[i].IO_num);
        }
        //each IO wait time
        arr[i].total_IO_burst_time = 0;
        
        if(arr[i].IO_num==0)
            arr[i].IO_burst_time = NULL;
        else
            arr[i].IO_burst_time = create_IO_burst(arr[i].IO_num, IO_time_limit, &arr[i].total_IO_burst_time);
        
       
        //arrival time
        int IO_start_sum = 0;
        if(start_at_0 == 1){
            arr[i].arrival_time = 0;
        }
        else{
            if(i==0)
                arr[i].arrival_time=0;
            else{     //has to be larger or equal than former process + before IO if exist
                    if(arr[i-1].IO_num==0){ //no IO
                        arr[i].arrival_time = (rand() % arr[i-1].cpu_burst_time) + arr[i-1].arrival_time;
                    }
                    else{
                        arr[i].arrival_time = (rand() % arr[i-1].IO_start[0]) + arr[i-1].arrival_time;
                    }
                }
        }

        //priority  larger -> higher priority, distinct
        arr[i].priority = random_priority[i];

        //initializing completion and analysis value
        arr[i].total_waiting_time = 0;
        arr[i].waiting_start = arr[i].arrival_time;
        arr[i].turnaround_time = 0; //complete time - arrival time = waiting + IO wait + cpuburst
        arr[i].response_time = 0;
        arr[i].IO_return_time = 0;
        arr[i].IO_req = 0;
    }

    free(random_priority);

    return arr;
}

void free_PCB(PCB* arr, int total_num){
    for(int i=0;i<total_num;i++){
        free(arr[i].IO_start);
        free(arr[i].IO_burst_time);
    }
    free(arr);
}

void reset_PCB(PCB* arr, int total_num){
    for(int i=0;i<total_num;i++){
        arr[i].total_waiting_time = 0;
        arr[i].waiting_start = arr[i].arrival_time;
        arr[i].turnaround_time = 0; //complete time - arrival time = waiting + IO wait + cpuburst
        arr[i].response_time = 0;
        arr[i].IO_return_time = 0;
        arr[i].IO_req = 0;
        arr[i].cpu_burst_used_time = 0;
    }
}