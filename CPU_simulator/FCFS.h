#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void FCFS_Scheduling(Process* processes, int num_processes);

void FCFS_Scheduling(Process* processes, int num_processes) {
    ///////////////////변수 선언
    int current_time = 0;
    int remaining_processes = num_processes;
    Process current_process;
    //Readyqueue 와 watiting queue 초기화
    Queue readyQueue;
    initializeQueue(&readyQueue, num_processes*2); //처음 올때 + I/O 갔다올때 - 총 2번 들어간다.
    Queue waitingQueue;
    initializeQueue(&waitingQueue, num_processes);
    bool working = false; //시작한 t 시점에 Cpu가 쉬는지 아닌지 확인
    bool worked = false; //t+1초에 CPU가 일을 하고 있는지 확인 아니라면 간트 차트에는 IDLE이 들어가야 한다.
    
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
        //I/o 가 끝난 것은 ready로 보내야하지만, 그러면 1초에 i/o작업1초 cpu작업 1초 하게 될 수 있다.
        //항상 작업 먼저 -> 이후에 queue이동해야 한다.
        //만약 cpu작업을 먼저 했다면 마찬가지로i/o로 보내는 일은 i/o 작업 이후에 두어야한다. 
        
        //CPU가 쉬고있고, readyqueue에 누가 있다면
        if(working == false && !isQueueEmpty(&readyQueue) ){
            current_process = dequeue(&readyQueue);
            working = true;
            worked = true;
            current_process.cpu_burst_time -= 1;
            //cpu 이용할 때마다 간트차트 기록
            gantt_chart[current_time] = current_process.pid;
            
            //I/O interrupts(처음 한번만 발생)
            if (current_process.io_burst_time > 0) {
                enqueue(&waitingQueue, current_process);
                working = false;
            }
            //끝난 프로세스 보내기
            if(current_process.cpu_burst_time == 0){
                //current_process말고 전달받은 파라미터 주소에 업데이트 해야한다.
                processes[current_process.pid].turnaround_time = current_time + 1- current_process.arrival_time; 
                remaining_processes--;
                working = false;
            }
        }
        //CPU가 이미 수행중이라면
        //여기 else 없어서 그거 찾는데만 1시간 넘게 걸림. 주의 요망
        else if(working ==true){
            //1초 수행
            current_process.cpu_burst_time -= 1;
            worked= true;
            gantt_chart[current_time] = current_process.pid;
            //끝났는지 확인
            if(current_process.cpu_burst_time == 0){
                processes[current_process.pid].turnaround_time = current_time + 1 - current_process.arrival_time;
                remaining_processes--;
                working = false;
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
        //이렇게 해주지 않으면 IDLE이어야 할 때 current_process pid가 간트 차트에 더 들어갈 수 있다.
        if(worked == false){
            gantt_chart[current_time] = -1;
        }
        // 시간 증가!
        current_time++;
        //초기화
        worked = false;
        
    }//while
    
    //대기 시간, turnaround_time 계산하여 출력하는 함수
    CalculateAverageTimes(processes, num_processes,1);
   
    //간트차트 출력하는 함수
    print_gantt_chart(gantt_chart, current_time);
    
    //메모리 해제
    free(gantt_chart);
    //readtqueue와 waiting queue는 실제 변수 선언한거라 메모리 해제 안한다.
}

