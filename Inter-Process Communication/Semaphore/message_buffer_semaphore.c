#include "message_buffer_semaphore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

int shmid;
void *memory_segment=NULL;

int semid;
union semun sem_union;

void init_sem() {
    /*---------------------------------------*/
    /* TODO 1 : init semaphore               */
    semid = semget(SEM_KEY, 1, IPC_CREAT|IPC_EXCL|0666);
    if (semid == -1) {
        printf("semget error!\n");
        exit(1);
    }

    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        printf("semctl error!\n");
        exit(1);
    }
    /* TODO 1 : END                          */
    /*---------------------------------------*/
    printf("init semid : %d\n", semid);
}

void destroy_sem() {
    /*---------------------------------------*/
    /* TODO 2 : destroy semaphore            */
    if (semctl(semid, 0, IPC_RMID) == -1) {
        printf("semctl error!\n");
        exit(1);
    }
    /* TODO 2 : END                          */
    /*---------------------------------------*/
}

void s_wait() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    semid = semget(SEM_KEY, 0, 0);

    if (semop(semid, &buf, 1) == -1) {
        printf("<s_wait> semop error!\n");
	return;
    }
}

void s_quit() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    semid = semget(SEM_KEY, 0, 0);

    if (semop(semid, &buf, 1) == -1) {
        printf("<s_quit> semop error!\n");
	return;
    }
}

/*---------------------------------------------*/
/* TODO 3 : use s_quit() and s_wait()          */
/* (1) clone from assignment 1-1               */
/* (2) use semaphore                           */
/* (3) report explanation (ex. 2020020000.pdf) */


int init_buffer(MessageBuffer **buffer) {
    //KEY 대신 SHM_KET로 변경
    key_t key = SHM_KEY;
    size_t size = sizeof(MessageBuffer);
    shmid = shmget(key, size, IPC_CREAT|0666);
    if (shmid == -1) {
        printf("shmget error!\n");
        return -1;
    }
    memory_segment = shmat(shmid, NULL, 0);
    if (memory_segment == (void *)-1) {
        printf("shmat error!\n");
        return -1;
    }
    *buffer = (MessageBuffer *)memory_segment;
    (*buffer)->is_empty = 0;

    printf("init buffer\n");
    return 0;
}

int attach_buffer(MessageBuffer **buffer) {

    (*buffer) = shmat(shmid, NULL, 0);
    if ( *buffer == (void *)-1) {
        printf("shmat error!\n");
        return -1;
    }
    printf("attach buffer\n");
    printf("\n");
    return 0;
}

int detach_buffer() {
    if (shmdt(memory_segment) == -1) {
        printf("shmdt error!\n\n");
        return -1;
    }
    printf("detach buffer\n\n");
    return 0;
}

int destroy_buffer() {
    if(shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("shmctl error!\n\n");
        return -1;
    }
    printf("destroy shared_memory\n\n");
    return 0;
}

int produce(MessageBuffer **buffer, int sender_id, int data, int account_id) {

    //공유메모리에 접근
    s_wait();

    //과제설명: 만약 ACCOUNT가 없는 계좌라면 여기서 S_QUIT()하고 RETURN 해줘야함.
    //과제설명: 그리고 만약 있는 금액보다 크게 출금하면 마찬가지로. 


    (*buffer)->account_id= account_id;
    (*buffer)->messages[(*buffer)->account_id].sender_id=sender_id;
    //기존에 바로 data에 더해주는 방식에서, 충돌을 보여주기 위해 반복문을 이용하는 방식으로 바꾸었다.
    //반복문으로 원하는 금액만큼 입출금
    for(int i =0; i < abs(data); i++ ){
        //입금
        if(data > 0){
            (*buffer)->messages[(*buffer)->account_id].data += 1;
        }
        //출금
        else{
            (*buffer)->messages[(*buffer)->account_id].data -= 1;
        }
    }
    (*buffer)->is_empty += 1;
    printf("produce message\n");
    //공유메모리 접근 끝남
    s_quit();

    return 0;
}

int consume(MessageBuffer **buffer, Message **message) {

    //과제설명 여기도 s_wait()
    if( (*buffer)->is_empty == 0){
        //s_quit
        return -1;
    }
    (*buffer)->is_empty -= 1;
    *message = &(*buffer)->messages[(*buffer)->account_id];
    //s_quit();
    return 0;
}
/* TODO 3 : END                                          */
/*-------------------------------------------------------*/
