#include <stdbool.h>
//성능비교를 위해 글로벌 변수 사용
extern float globalArray[13];
//////////////////////////////////////변수 내용//////////////////////////////////////
//process 구조
typedef struct {
    int pid;
    int arrival_time;
    int cpu_burst_time;
    int io_burst_time;
    int priority;
    int waiting_time;
    int turnaround_time;
} Process;


// Queue 구조
typedef struct {
    Process* processes;
    int front;
    int rear;
    int capacity;
} Queue;

// Heap 구조. 
typedef struct {
    Process* processes;
    int capacity;
    int size;
} BinaryHeap;


//////////////////////////////////////함수 헤드//////////////////////////////////////
void initializeQueue(Queue* queue, int capacity);
bool isQueueEmpty(Queue* queue);
void enqueue(Queue* queue, Process process);
Process dequeue(Queue* queue);
void print_gantt_chart(int* gantt_chart, int current_time);
void swap(Process* a, Process* b);
Process extractMinProcess(BinaryHeap* heap);
void insertProcess(BinaryHeap* heap, Process process);
void heapify(BinaryHeap* heap, int index);
void swapProcesses(Process* a, Process* b);
BinaryHeap* createBinaryHeap(int capacity);
bool isBinaryHeapEmpty(BinaryHeap* heap) ;
Process extractMinProcess_p(BinaryHeap* heap);
void insertProcess_p(BinaryHeap* heap, Process process);
void heapify_p(BinaryHeap* heap, int index);


//////////////////////////////////////함수 내용//////////////////////////////////////
//프로세스 생성 함수
//한 번 생성한 데이터를 반환해, main에서 저장해두고, 이를 각각의 스케줄러에게 넘긴다. 
Process* Create_Process(int num_processes) {
    //몇 개 만들지 받아서 개수만큼 만들어준다.
    Process* processes = (Process*)malloc(num_processes * sizeof(Process));
    if (processes == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    
    int i;
    srand(time(NULL)); //랜덤 변수를 생성하기 위한 seed
    //변하지 않는 seed를 주면 한번 생성한 랜덤 variable이 계속된다.
    
    for (i = 0; i < num_processes; i++) {
        processes[i].pid = i + 1; // 유니크한 ID
        processes[i].arrival_time = rand() % 10; // 0 ~ 9
        processes[i].cpu_burst_time = rand() % 10 + 2; // 2 ~ 11  1초면 CPU 끝나고 I/O가 발생하는 상황이니까 뺀다.   
        processes[i].io_burst_time = rand() % 5 + 1; // 1 ~ 5 무조건 발생하도록
        processes[i].priority = rand() % 7 + 1; // 1~7 의 우선순위로, heap함수 수정을 최소화하기 위해 작을수록 우선한다. 
        processes[i].waiting_time = 0;//초기화
        processes[i].turnaround_time = 0;
    }
    return processes;
}

// C라이브러리의 qsort를 이용하기 위한 comparison function
// 다양한 데이터를 받기 위해, qsort는 const void 형태의 주소를 통해 두 데이터를 비교하는 함수를 요구한다.
int CompareByArrivalTime(const void* a, const void* b) {
    const Process* processA = (const Process*)a;
    const Process* processB = (const Process*)b;

    if (processA->arrival_time < processB->arrival_time) {
        return -1;  //A가 더 빨리 도착
    } else if (processA->arrival_time > processB->arrival_time) {
        return 1;   //B가 더 빨리 도착
    }

    return 0; // 같이 도착
}


///////////////////////// evaluation 함수
void CalculateAverageTimes(Process* processes, int num_processes, int algorithmIndex) {
    /*
    printf("Debug: num_processes = %d\n", num_processes);
    for (int i = 0; i < num_processes; i++) {
        printf("Debug: processes[%d].waiting_time = %d\n", i, processes[i].waiting_time);
        printf("Debug: processes[%d].turnaround_time = %d\n", i, processes[i].turnaround_time);
    }
    */
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    for (int i = 0; i < num_processes; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;

    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    
    
    //성능비교
    globalArray[algorithmIndex*2 - 1] = avg_waiting_time;
    globalArray[algorithmIndex*2] = avg_turnaround_time;
    
    return;
}

//////////////////////////////간트차트
void print_gantt_chart(int* gantt_chart, int current_time){
      // Print Gantt chart
    printf("Gantt Chart:\n");
    //Vm 화면이 작으니까 각줄 => 10개씩 반복
    for (int i = 0; i <= current_time/10 ; i++) {
    //0~9 / 10~19
        for (int j = 0 + 10*i ; j < 10 + 10*i ; j++) {
            if(j > current_time) break;
            printf("%-6d", j);
        }
        printf("\n");
        for (int j = 0 + 10*i; j < 10 + 10*i ; j++) {
            if(j > current_time) break;
            if (gantt_chart[j] == -1) {
                printf("IDLE  ");
            } 
            else {
                //실제 손으로 그린 간트차트와 달리 n초 지점 경계가 애매하니까 마지막 1초는 그냥 빈칸으로 
                if(j == current_time) printf(" ");
                else printf("P%-5d", gantt_chart[j]);
            }  
        }
        printf("\n\n");
    }
}



///////////////////////////////////////큐 관련 함수들
// Function to initialize a queue
void initializeQueue(Queue* queue, int capacity) {
    queue->processes = (Process*)malloc(capacity * sizeof(Process));
    queue->front = 0;
    queue->rear = -1;
    queue->capacity = capacity;
}

// Function to check if the queue is empty
bool isQueueEmpty(Queue* queue) {
    return (queue->rear - queue->front + 1) == 0;
}

// Function to enqueue a process into the queue
void enqueue(Queue* queue, Process process) {
    if (queue->rear == queue->capacity - 1) {
        printf("Queue is full.\n");
        exit(1);
    }

    queue->rear++;
    queue->processes[queue->rear] = process;
}

// Function to dequeue a process from the queue
Process dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty.\n");
        exit(1);
    }
    Process process = queue->processes[queue->front];
    queue->front++;
    return process;
}



//////////////////////////////////////////////힙 관련 함수들 - cpu_burst_time 기준
// Function to create a new binary heap
BinaryHeap* createBinaryHeap(int capacity) {
    BinaryHeap* heap = (BinaryHeap*)malloc(sizeof(BinaryHeap));
    heap->processes = (Process*)malloc(capacity * sizeof(Process));
    heap->capacity = capacity;
    heap->size = 0;
    return heap;
}

// Function to swap two processes
void swapProcesses(Process* a, Process* b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

// Function to maintain the min-heap property of the binary heap
void heapify(BinaryHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->processes[left].cpu_burst_time < heap->processes[smallest].cpu_burst_time) {
        smallest = left;
    }

    if (right < heap->size && heap->processes[right].cpu_burst_time < heap->processes[smallest].cpu_burst_time) {
        smallest = right;
    }

    if (smallest != index) {
        swapProcesses(&heap->processes[index], &heap->processes[smallest]);
        heapify(heap, smallest);
    }
}

// Function to insert a process into the binary heap
void insertProcess(BinaryHeap* heap, Process process) {
    if (heap->size == heap->capacity) {
        return; // Heap is full, cannot insert
    }

    int index = heap->size;
    heap->processes[index] = process;
    heap->size++;

    while (index > 0 && heap->processes[index].cpu_burst_time < heap->processes[(index - 1) / 2].cpu_burst_time) {
        swapProcesses(&heap->processes[index], &heap->processes[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

// Function to extract the process with the shortest CPU burst time from the binary heap
Process extractMinProcess(BinaryHeap* heap) {
    if (heap->size == 0) {
        Process emptyProcess;
        emptyProcess.pid = -1;
        return emptyProcess; // Heap is empty
    }

    Process minProcess = heap->processes[0];
    heap->processes[0] = heap->processes[heap->size - 1];
    heap->size--;

    heapify(heap, 0);

    return minProcess;
}

bool isBinaryHeapEmpty(BinaryHeap* heap) {
    return (heap->size == 0);
}


//////////////////////////////////////////////힙 관련 함수들 - priority 기준

void heapify_p(BinaryHeap* heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->processes[left].priority < heap->processes[smallest].priority) {
        smallest = left;
    }

    if (right < heap->size && heap->processes[right].priority < heap->processes[smallest].priority) {
        smallest = right;
    }

    if (smallest != index) {
        swapProcesses(&heap->processes[index], &heap->processes[smallest]);
        heapify_p(heap, smallest);
    }
}

// Function to insert a process into the binary heap
void insertProcess_p(BinaryHeap* heap, Process process) {
    if (heap->size == heap->capacity) {
        return; // Heap is full, cannot insert
    }

    int index = heap->size;
    heap->processes[index] = process;
    heap->size++;

    while (index > 0 && heap->processes[index].priority < heap->processes[(index - 1) / 2].priority) {
        swapProcesses(&heap->processes[index], &heap->processes[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

// Function to extract the process with the smallest priority == 1순위부터
Process extractMinProcess_p(BinaryHeap* heap) {
    if (heap->size == 0) {
        Process emptyProcess;
        emptyProcess.pid = -1;
        return emptyProcess; // Heap is empty
    }

    Process minProcess = heap->processes[0];
    heap->processes[0] = heap->processes[heap->size - 1];
    heap->size--;

    heapify_p(heap, 0);

    return minProcess;
}