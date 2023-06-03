#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void Priority_NP_Scheduling(Process* processes, int num_processes);

void Priority_NP_Scheduling(Process* processes, int num_processes) {
    ///////////////////변수 선언
    int current_time = 0;
    int remaining_processes = num_processes;
    bool working = false; //Cpu가 쉬는지 아닌지 확인
    bool worked = false;
    Process current_process;
    
    //ready 힙. waiting queue 자료구조 이용
    BinaryHeap* readyHeap = createBinaryHeap(num_processes * 2); 
    Queue waitingQueue;
    initializeQueue(&waitingQueue, num_processes);

    // 간트 차트 저장하기 위함. 
    // CPU busrt가 최대 11이고 IDEL이 약간 발생가능 -> 프로세스 개수 * 20이면 크기는 충분할 것 같다. 
    int* gantt_chart = malloc( num_processes * 20 * sizeof(int));
    for (int i = 0; i <= current_time; i++) {
        gantt_chart[i] = -1; // -1 = (idle state)
    }
    
    while ( remaining_processes > 0 ||  !isBinaryHeapEmpty(readyHeap)) {
        //도착 시간에 따라 readyqueue에 넣는다. 
        for (int i = 0; i < num_processes ; i++) {
            if ( processes[i].arrival_time == current_time) {
                insertProcess_p(readyHeap, processes[i]);
            }
        }
        
        //I/O operation 역시 매초 진행
        if(!isQueueEmpty(&waitingQueue)){
            //1초 진행
            waitingQueue.processes[waitingQueue.front].io_burst_time--;

        }
      
        //CPU가 쉬고있고, readyqueue에 누가 있다면
        if(working == false && !isBinaryHeapEmpty(readyHeap) ){
            //가장 짧은 cpu_burst_time 고른다.
            current_process = extractMinProcess_p(readyHeap);
            working = true;
            current_process.cpu_burst_time -= 1;
            worked = true;
            //cpu 이용할 때마다 간트차트
            gantt_chart[current_time] = current_process.pid;
            
            //처음 한번만 발생하는 I/O interrupts
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
            worked = true;
            gantt_chart[current_time] = current_process.pid;
            //끝났는지 확인
            if(current_process.cpu_burst_time == 0){
                processes[current_process.pid].turnaround_time = current_time + 1 - current_process.arrival_time;
                remaining_processes--;
                working = false;
            }
        }
        
        //waiting time 계산, readyqueue에서 기다린 시간만 포함
       for (int i = 0; i <= readyHeap->size - 1; i++) {
            processes[readyHeap->processes[i].pid].waiting_time++;
        }
        //I/O 작업이 끝났으면 waitingQueue에서 빼서 readyQueue로 보낸다.
        if(!isQueueEmpty(&waitingQueue) && waitingQueue.processes[waitingQueue.front].io_burst_time == 0){
            Process completedProcess = dequeue(&waitingQueue);
            insertProcess_p(readyHeap, completedProcess);
        }
        if(worked == false){
            gantt_chart[current_time] = -1;
        }
        // 시간 증가!
        current_time++;
        worked = false;
        
    }//while
    
    //대기 시간, turnaround_time 계산하여 출력하는 함수
    CalculateAverageTimes(processes, num_processes,4);
   
    //간트차트 출력하는 함수
    print_gantt_chart(gantt_chart, current_time);
    
    //메모리 해제
    free(gantt_chart);
    free(readyHeap->processes);
    free(readyHeap);
}