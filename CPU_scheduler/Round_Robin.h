#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void RR_Scheduling(Process* processes, int num_processes, int time_quantum);

void RR_Scheduling(Process* processes, int num_processes, int time_quantum) {
    ///////////////////변수 선언
    int current_time = 0;
    int remaining_processes = num_processes;
    Process current_process;
    //제한시간을 각자 갖고 있는게 아니라, 공유하는 식으로
    int time_limit = time_quantum; // RR 만의 변수
    
    //Readyqueue 와 watiting queue 초기화
    Queue readyQueue;
    initializeQueue(&readyQueue, num_processes*(time_quantum+2) ); //RR방식은 여러번 readyqueue로가니까 더 크게 
    Queue waitingQueue;
    initializeQueue(&waitingQueue, num_processes);
    bool working = false; //Cpu가 쉬는지 아닌지 확인
    bool worked = false;
    
    // 간트 차트 저장하기 위함. 
    // CPU busrt가 최대 11이고 IDEL이 약간 발생가능 -> 프로세스 개수 * 20이면 크기는 충분할 것 같다. 
    int* gantt_chart = malloc( num_processes * 20 * sizeof(int));
    for (int i = 0; i <= current_time; i++) {
        gantt_chart[i] = -1; // -1 = (idle state)
    }
    
    while ( remaining_processes > 0 ||  !isQueueEmpty(&readyQueue)) {
        //도착 시간에 따라 readyqueue에 넣는다. 
        for (int i = 0; i < num_processes ; i++) {
            if ( processes[i].arrival_time == current_time) {
                enqueue(&readyQueue, processes[i]);
            }
        }
        
        //I/O operation 역시 매초 진행
        if(!isQueueEmpty(&waitingQueue)){
            //1초 진행
            waitingQueue.processes[waitingQueue.front].io_burst_time--;
        }
        
        //CPU가 쉬고있고, readyqueue에 누가 있다면
        if(working == false && !isQueueEmpty(&readyQueue) ){
            current_process = dequeue(&readyQueue);
            working = true;
            worked = true;
            current_process.cpu_burst_time -= 1;            
            gantt_chart[current_time] = current_process.pid;
            time_limit--; //cpu 쓸 때마다 시간 제한 -1초
            //처음 한번만 발생하는 I/O interrupts
            if (current_process.io_burst_time > 0) {
                enqueue(&waitingQueue, current_process);
                working = false;
                time_limit = time_quantum; // 시간 제한 초기화
            }
            //끝난 프로세스 보내기
            if(current_process.cpu_burst_time == 0){
                processes[current_process.pid].turnaround_time = current_time + 1- current_process.arrival_time; 
                remaining_processes--;
                working = false;
                time_limit = time_quantum; //시간 제한 초기화
            }
            //시간 제한이 1초 일 수도 있다.
            if(time_limit == 0){
                working = false;
                time_limit = time_quantum;
                enqueue(&readyQueue, current_process);
            }
        }
        //CPU가 이미 수행중이라면
        else if(working == true){
            //1초 수행
            current_process.cpu_burst_time -= 1;
            gantt_chart[current_time] = current_process.pid;
            worked = true;
            time_limit--;
            
            //끝났는지 확인
            if(current_process.cpu_burst_time == 0){
                processes[current_process.pid].turnaround_time = current_time + 1 - current_process.arrival_time;
                remaining_processes--;
                working = false;
                time_limit = time_quantum; // 시간제한 초기화
            }
            //끝나진 않았지만 시간 제한에 걸렸다면 보낸다.
            else if(time_limit == 0){
                working = false;
                time_limit = time_quantum;
                enqueue(&readyQueue, current_process);
            }
        }
        
        //waiting time 계산, readyqueue에서 기다린 시간만 포함
        for(int i=readyQueue.front; i<=readyQueue.rear ; i++ ){
            //readyQueue와 waitingQueue는 이 함수에서만 작동하므로
            //직접 함수 파라미터로 전달 받은 곳에 업데이트해야한다.
            processes[readyQueue.processes[i].pid].waiting_time++; 
        }
         //I/o 끝난 프로세스는 waitingQueue에서 빼서 readyQueue로 보낸다.
        if(!isQueueEmpty(&waitingQueue) && waitingQueue.processes[waitingQueue.front].io_burst_time == 0){
            Process completedProcess = dequeue(&waitingQueue);
            enqueue(&readyQueue, completedProcess);
        }
        if(worked == false){
            gantt_chart[current_time] = -1;
        }
        // 시간 증가!
        current_time++;
        worked = false;
        
    }//while
    
    
    //대기 시간, turnaround_time 계산하여 출력하는 함수
    CalculateAverageTimes(processes, num_processes,6);
   
    //간트차트 출력하는 함수
    print_gantt_chart(gantt_chart, current_time);
    
    //메모리 해제
    free(gantt_chart);
}

