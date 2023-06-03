#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define BUFFER_SIZE 100
#define NP_SEND "./server_to_client"
#define NP_RECEIVE "./client_to_server"

int main(void) {
	char receive_msg[BUFFER_SIZE], send_msg[BUFFER_SIZE];
	int receive_fd, send_fd;
	int value;

	/*---------------------------------------*/
	/* TODO 3 : make pipes and init fds      */
    /* (1) make NP_RECEIVE pipe              */

    //기존의 파이프 제거
    if(access(NP_RECEIVE, F_OK) ==0 ){
		if (unlink(NP_RECEIVE) == -1) {
            printf("Failed to remove existing named pipe.\n");
            return -1;
        }
	}
	//make client_to_server pipe
    if(mkfifo( NP_RECEIVE, 0666 ) == -1 ){
		perror("Error creating named pipe NP_RECEIVE");
		return -1;
	}

    /* (2) make NP_SEND pipe                 */
	  //기존의 파이프 제거
    if(access(NP_SEND, F_OK) ==0 ){
		if (unlink(NP_SEND) == -1) {
            printf("Failed to remove existing named pipe.\n");
            return -1;
        }
	}
    if(mkfifo(NP_SEND, 0666) == -1){
		printf("make fifo NP_SEND failed\n");
		return -1;
	}

    /* (3) init receive_fd and send_fd       */
	//처음에 오류가 나서 어디인지 보려고 다 나눠서 오류 처리
    receive_fd = open(NP_RECEIVE, O_RDONLY);
	if (receive_fd == -1) {
        printf("From server.c Recieive: Failed to open named pipes.\n");
        return -1;
    }
	//make server_to_client pipe
    send_fd = open(NP_SEND, O_WRONLY);
    if (send_fd == -1) {
        printf("From server.c Send: Failed to open named pipes.\n");
        return -1;
    }


	/* TODO 3 : END                          */
	/*---------------------------------------*/

	while (1) {
		/*---------------------------------------*/
		/* TODO 4 : read msg                     */
        ir( read(receive_fd, receive_msg, BUFFER_SIZE) == -1 ){
            printf("read error server");
        }
		/* TODO 4 : END                          */
		/*---------------------------------------*/

		printf("server : receive %s\n", receive_msg);

		value = atoi(receive_msg);

		sprintf(send_msg, "%d", value*value);
		printf("server : send %s\n", send_msg);

		/*---------------------------------------*/
		/* TODO 5 : write msg                    */
        if( write(send_fd, send_msg, strlen(send_msg) + 1) == -1 ){
            printf("write error server");
            return -1;
        }
		/* TODO 5 : END                          */
		/*---------------------------------------*/
	}
	return 0;
}
