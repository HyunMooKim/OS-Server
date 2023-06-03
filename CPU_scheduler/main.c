#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memcpy
#include <time.h> //random 프로세스 생성을 위해
#include "cpu_header.h"
#include "FCFS.h"
#include "SJF_NP.h"
#include "Priority_NP.h"
#include "Round_Robin.h"
#include "SJF_P.h"
#include "priority_P.h"

//성능 비교를 맨 마지막에 넣느라 전역변수 -> 평가 함수에서 각 알고리즘 성능을 저장
float globalArray[13]={0.0};

int main()
{
    int num_processes = 0;
    int menu = 0;
    //성능 출력을 위함
    const char* algorithmNames[] = {
    "",           // Index 0 제외
    "FCFS",
    "SJF_NP",
    "SJF_P",
    "Priority_NP",
    "Priority_P",
    "RR"
    };
    printf("how many datas?\n");
    
    //데이터 개수 받기
    scanf("%d", &num_processes);
    
    //original_processes 만들기. 이 때 만들어진 데이터 그대로 6개 스케줄링을 해야 비교 가능하다.
    Process* original_processes = Create_Process(num_processes);
    //도착시간에 맞게 정렬하기 - stdlib의 함수 사용, general 한 사용을 위해 자료형이 void이다. 따라서 크기 비교하는 함수 만들어줘야함.
    qsort(original_processes, num_processes, sizeof(Process), CompareByArrivalTime);

    ///////////////////////////프로세스 생성 및 정렬 확인 출력
    printf("PID\tArrival Time\tCPU Burst Time\tI/O Burst Time\tPriority\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", original_processes[i].pid, original_processes[i].arrival_time,
               original_processes[i].cpu_burst_time, original_processes[i].io_burst_time, original_processes[i].priority);
    }
    printf("\nyour data is here.\n");


    
    while(1){
        printf("\n----------------Choose your menu--------------\n");
        printf("0:exit 1:FCFS 2:SJF 3:SJF(Preemptive) 4:Prioiry 5:Priority(Preemptive) 6.Round Robin 7.evaluation\n");
        scanf("%d", &menu);
        
         //각 알고리즘에 똑같은 original_processes를 넣기 위해 algorithm_processes에 복사
        Process* algorithm_processes = (Process*)malloc(num_processes * sizeof(Process));
        if(algorithm_processes == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        memcpy(algorithm_processes, original_processes, num_processes * sizeof(Process)); 
        
        if(menu==0){
            break;
        }
        //FCFS
        else if(menu==1){
            printf("==========================FCFS============================\n");
            FCFS_Scheduling(algorithm_processes, num_processes);
            printf("\n");
        }
        //Non-Preemptive SJF 
        else if(menu==2){
            printf("==========================Non-Preemptive SJF==========================\n");
            SJF_NP_Scheduling(algorithm_processes, num_processes);
            printf("\n");
        }
        //Preemptive SJF 
        else if(menu==3){
            printf("==========================Preemptive SJF  SJF==========================\n");
            SJF_P_Scheduling(algorithm_processes, num_processes);
            printf("\n");
        }
        //Non-Preemptive Priority
        else if(menu==4){
            printf("==========================Non-Preemptive Priority==========================\n");
            Priority_NP_Scheduling(algorithm_processes, num_processes);
            printf("\n");
        }
        //Preemptive priority
        else if(menu==5){
            printf("========================== Preemptive prioirity ==========================\n");
            Priority_P_Scheduling(algorithm_processes, num_processes);
            printf("\n");
        }
        //Round Robin 
        else if(menu==6){
            printf("==========================Round Robin==========================\n");
            RR_Scheduling(algorithm_processes, num_processes, 3); //time quantum을 알려줘야한다. 
            printf("\n");
        }
        else if(menu==7){
            printf("==========================Evaluation==========================\n");
            for(int i = 1 ; i<=6; i++){
                printf("%s\t", algorithmNames[i]);
                //1,2 = FCFS의 waitingtime, turnaroundtime
                printf("%f %f\n",globalArray[2*i - 1], globalArray[2 *i]);
            } 
            printf("\n");
            float maxWaitingTime = -1.0;
            float minWaitingTime = 99999.9;
            float maxTurnaroundTime = -1.0;
            float minTurnaroundTime = 99999.9;
            int maxWaitingIndex = -1;
            int minWaitingIndex = -1;
            int maxTurnaroundIndex = -1;
            int minTurnaroundIndex = -1;

            for (int i = 1; i <= 6; i++) {
                float waitingTime = globalArray[2 * i - 1];
                float turnaroundTime = globalArray[2 * i];

                if (waitingTime > maxWaitingTime) {
                    maxWaitingTime = waitingTime;
                    maxWaitingIndex = i;
                }

                if (waitingTime < minWaitingTime) {
                    minWaitingTime = waitingTime;
                    minWaitingIndex = i;
                }

                if (turnaroundTime > maxTurnaroundTime) {
                    maxTurnaroundTime = turnaroundTime;
                    maxTurnaroundIndex = i;
                }

                if (turnaroundTime < minTurnaroundTime) {
                    minTurnaroundTime = turnaroundTime;
                    minTurnaroundIndex = i;
                }
            }

   
            printf("\nMaximum Waiting Time:%s %.2f\n", algorithmNames[maxWaitingIndex],maxWaitingTime);
            printf("Minimum Waiting Time:%s %.2f\n", algorithmNames[minWaitingIndex],minWaitingTime);
            printf("Maximum Turnaround Time:%s %.2f\n", algorithmNames[maxTurnaroundIndex],maxTurnaroundTime);
            printf("Minimum Turnaround Time:%s %.2f\n", algorithmNames[minTurnaroundIndex],minTurnaroundTime);
                
        }
        else{
            printf("wrong menu\n");
        }

        free(algorithm_processes);
        
    }//while
    


    //메모리 해제
    free(original_processes);

    return 0;
}
