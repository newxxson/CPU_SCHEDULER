#include <stdio.h>
#include "scheduler/scheduler.h"
#include "config/config.h"
#include <stdbool.h>
#include <stdlib.h>





void sortascending(char* scheduler_arr[], float* time_arr[], int scheduler_num){
    int i,j; 
    char* scheduler_temp;
    float time_temp;
    for(i=0;i<scheduler_num;i++){
        for(j=0;j<scheduler_num-i-1;j++){
            if(*(time_arr[j]) > *(time_arr[j+1])){
                time_temp = *(time_arr[j]);
                *(time_arr[j]) = *(time_arr[j+1]);
                *(time_arr[j+1]) = time_temp;
                
                scheduler_temp = scheduler_arr[j];
                scheduler_arr[j] = scheduler_arr[j+1];
                scheduler_arr[j+1] = scheduler_temp;
            }
        }
    }
}

void compare_eval(char* scheduler_arr[], float* time_arr[], int scheduler_num, int mode){
    //RT
    if(mode == 0){
        printf("Response time\n\n");
        for(int i=0;i<scheduler_num;i++){
            printf("%d. %s with RT : %.2f\n", i, scheduler_arr[i], *(time_arr[i]));
        }
    }//TT
    else if(mode == 1){
        printf("Turnaround time\n");
        for(int i=0;i<scheduler_num;i++){
            printf("%d. %s with TT : %.2f\n", i, scheduler_arr[i], *(time_arr[i]));
        }
    }//WT
    else if(mode == 2){
        printf("Total waiting time\n");
        for(int i=0;i<scheduler_num;i++){
            printf("%d. %s with WT : %.2f\n", i, scheduler_arr[i], *(time_arr[i]));
        }
    }
    printf("\n");
}

void print_PCB(PCB*arr, int total_num){
    printf("|process_number|arrival_time|cpu_burst|IO_num|priority|\n");
    for(int i=0;i<total_num;i++){
        char str[4];
        sprintf(str, "P%d", arr[i].process_num);
        printf("|%*s|%*d|%*d|%*d|%*d|\n", 14, str, 12, arr[i].arrival_time, 9, arr[i].cpu_burst_time, 6, arr[i].IO_num, 8, arr[i].priority);
    }
    printf("\n\n");
}

void check_eval(PCB* arr, int total_num, float* RT, float* TT, float* WT){
    printf("|process_number|cpu_burst|response_time|Turnaround_time|total_WT|\n");
    int total_RT=0, total_TT=0, total_WT=0;
    for(int i=0;i<total_num;i++){
        char str[4];
        sprintf(str, "P%d", arr[i].process_num);
        //to secure first processes RT
        if(arr[i].response_time == -1)
            arr[i].response_time = 0;

        printf("|%*s|%*d|%*d|%*d|%*d|\n", 14, str, 9, arr[i].cpu_burst_time, 13, arr[i].response_time, 15, arr[i].turnaround_time, 8, arr[i].total_waiting_time);
        total_RT += arr[i].response_time;
        total_TT += arr[i].turnaround_time;
        total_WT += arr[i].total_waiting_time;
    }
    *RT = (float)total_RT/total_num;
    *TT = (float)total_TT/total_num;
    *WT = (float)total_WT/total_num;
    printf("Average response time: %.2f, Average turnaround time: %.2f, Average waiting time: %.2f\n\n\n", *RT, *TT, *WT);
}


void test(PCB * arr, int total_num, int mode){
    //before eval 0
    for(int i=0;i<total_num;i++){
        if(mode == 0){
            // printf("pid : %d\n", arr[i].pid);
            printf("process_number : %d\n", arr[i].process_num);
            printf("arrival_time : %d\n", arr[i].arrival_time);
            printf("cpu_burst_time : %d\n", arr[i].cpu_burst_time);
            printf("IO_num : %d\n", arr[i].IO_num);
            printf("====================\n");
            if(arr[i].IO_num > 0){
                for(int k=0;k<arr[i].IO_num;k++){
                    printf("IO_start[%d] : %d &", k,(arr[i]).IO_start[k]);
                    printf("IO_lenght[%d]: %d", k, (arr[i]).IO_burst_time[k]);
                }
            }
            printf("\n\n");
        }//after eval
        else if(mode == 1){
            printf("======================\n");
            printf("process_number : %d\n", (arr[i]).process_num);
            printf("turnaround time: %d\n", (arr[i]).turnaround_time);
            printf("total waiting time: %d\n\n\n", (arr[i]).total_waiting_time);
        }
    }
}

void get_params(int* total_num, int* cpu_limit, int* IO_limit, int* IO_time_limit, int* display, int mode){
    printf("number of process: ");
    scanf("%d", total_num);
    printf("limit of cpu burst time: ");
    scanf("%d", cpu_limit);
    printf("limit of IO request time: ");
    scanf("%d", IO_limit);
    printf("limit of IO burst time: ");
    scanf("%d", IO_time_limit);
    printf("print details? 1 for true 0 for false: ");
    scanf("%d", display);
}



int main(){
    int mode;
    int total_num=6, cpu_limit=15, IO_limit=3, IO_time_limit=10, display=1;
    float FCFS_RT, FCFS_TT, FCFS_WT;
    float SJF_RT, SJF_TT, SJF_WT;
    float P_SJF_RT, P_SJF_TT, P_SJF_WT;
    float PRIORITY_RT, PRIORITY_TT, PRIORITY_WT;
    float P_PRIORITY_RT, P_PRIORITY_TT, P_PRIORITY_WT;
    float RR_RT, RR_TT, RR_WT;
    float MLQ_RT, MLQ_TT, MLQ_WT;
    int time_quantum;
    float criterion;
    int scheduler_num = 7;
    char* RT_scheduler_arr[] = {"FCFS", "non Preem SJF", "non Preem Priority", "Preem SJF", "Preem Priority", "RR", "MLQ"};
    char* TT_scheduler_arr[] = {"FCFS", "non Preem SJF", "non Preem Priority", "Preem SJF", "Preem Priority", "RR", "MLQ"};
    char* WT_scheduler_arr[] = {"FCFS", "non Preem SJF", "non Preem Priority", "Preem SJF", "Preem Priority", "RR", "MLQ"};
    float* RT_arr[] = {&FCFS_RT, &SJF_RT, &PRIORITY_RT, &P_SJF_RT, &P_PRIORITY_RT, &RR_RT, &MLQ_RT};
    float* TT_arr[] = {&FCFS_TT, &SJF_TT, &PRIORITY_TT, &P_SJF_TT, &P_PRIORITY_TT, &RR_TT, &MLQ_TT};
    float* WT_arr[] = {&FCFS_WT, &SJF_WT, &PRIORITY_WT, &P_SJF_WT, &P_PRIORITY_WT, &RR_WT, &MLQ_WT};
    printf("Check which algorithm to run\n");
    printf("0 for all. 1 for FCFS. 2 for non Preem. SJF. 3 for non Preem PRIORITY. 4 for Preem SJF. 5 for Preem PRIORITY. 6 for RR. 7 for MLQ\n");
    scanf("%d", &mode);

    if(mode==0){
        printf("Set Time Quantum for RR and MLQ: ");
        scanf("%d", &time_quantum);
        printf("Set criterion for dividing processes by length in MLQ: ");
        scanf("%f", &criterion);
    }
    get_params(&total_num, &cpu_limit, &IO_limit, &IO_time_limit, &display, mode);

    int start_at_0;
    printf("Should all the process start at 0? 1 for true : ");
    scanf("%d", &start_at_0);

    PCB* arr = Create_Process(total_num, cpu_limit, IO_limit, IO_time_limit, start_at_0);
    
    
    
    
    print_PCB(arr, total_num);
    // test(arr, total_num, 0);

    switch (mode)
    {
    case 0:
        printf("--------FCFS--------\n");
        FCFS(&arr, total_num, display);
        check_eval(arr, total_num, &FCFS_RT, &FCFS_TT, &FCFS_WT);

        reset_PCB(arr, total_num);

        // print_PCB(arr, total_num);
        
        printf("--------SJF--------\n");
        SJF(&arr, total_num, display);
        check_eval(arr, total_num, &SJF_RT, &SJF_TT, &SJF_WT);

        reset_PCB(arr, total_num);

        // print_PCB(arr, total_num);
        
        printf("--------PRIORITY--------\n");
        PRIORITY(&arr, total_num, display);
        check_eval(arr, total_num, &PRIORITY_RT, &PRIORITY_TT, &PRIORITY_WT);

        reset_PCB(arr, total_num);

        // print_PCB(arr, total_num);
        
        printf("--------Preemptive SJF--------\n");
        P_SJF(&arr, total_num, display);
        check_eval(arr, total_num, &P_SJF_RT, &P_SJF_TT, &P_SJF_WT);

        reset_PCB(arr, total_num);

        // print_PCB(arr, total_num);
        
        printf("--------Preemtive PRIORITY--------\n");
        P_PRIORITY(&arr, total_num, display);
        check_eval(arr, total_num, &P_PRIORITY_RT, &P_PRIORITY_TT, &P_PRIORITY_WT);
        
        reset_PCB(arr, total_num);

        // print_PCB(arr, total_num);
        
        printf("--------RR--------\n");
        RR(&arr, total_num, time_quantum, display);
        check_eval(arr, total_num, &RR_RT, &RR_TT, &RR_WT);

        reset_PCB(arr, total_num);
        
        printf("--------MLQ--------\n");
        MLQ(&arr, total_num, criterion, time_quantum, display);
        
        check_eval(arr, total_num, &MLQ_RT, &MLQ_TT, &MLQ_WT);

        sortascending(RT_scheduler_arr, RT_arr, scheduler_num);
        sortascending(TT_scheduler_arr, TT_arr, scheduler_num);
        sortascending(WT_scheduler_arr, WT_arr, scheduler_num);
        compare_eval(RT_scheduler_arr, RT_arr, scheduler_num, 0);
        compare_eval(TT_scheduler_arr, TT_arr, scheduler_num, 1);
        compare_eval(WT_scheduler_arr, WT_arr, scheduler_num, 2);

        break;
    case 1:
        FCFS(&arr, total_num, display);
        check_eval(arr, total_num, &FCFS_RT, &FCFS_TT, &FCFS_WT);
        break;
    case 2:
        SJF(&arr, total_num, display);
        check_eval(arr, total_num, &SJF_RT, &SJF_TT, &SJF_WT);
        break;
    case 3:
        PRIORITY(&arr, total_num, display);
        check_eval(arr, total_num, &PRIORITY_RT, &PRIORITY_TT, &PRIORITY_WT);
        break;
    case 4:
        P_SJF(&arr, total_num, display);
        check_eval(arr, total_num, &P_SJF_RT, &P_SJF_TT, &P_SJF_WT);
        break;
    case 5:
        P_PRIORITY(&arr, total_num, display);
        check_eval(arr, total_num, &P_PRIORITY_RT, &P_PRIORITY_TT, &P_PRIORITY_WT);
        break;
    case 6:
        printf("Set Time quantum to : ");
        scanf("%d", &time_quantum);
        RR(&arr, total_num, time_quantum, display);
        check_eval(arr, total_num, &RR_RT, &RR_TT, &RR_WT);
        break;
    case 7:
        printf("Set criterion for dividing processes by length: ");
        scanf("%f", &criterion);
        printf("Set Time quantum to : ");
        scanf("%d", &time_quantum);
        MLQ(&arr, total_num, criterion, time_quantum, display);
        check_eval(arr, total_num, &MLQ_RT, &MLQ_TT, &MLQ_WT);

    }
    display = 0;
    
    free_PCB(arr, total_num);
    return 0;
}