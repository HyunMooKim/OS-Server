#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#define MAXCLNT 25
#define LENBUFF 256

void errProc(const char*);
int processMsg (char *rBUff, int readLen, int sock);
int transmitReply (int sock);
int sendFile(FILE *fp);
int readLineFromMsg(char *out, int *point, char* in);
int parseByNullMsg(char *out, int *point, char* in);
void SendErrMsg(int sock);
void SendErrMsg_400(int sock);
char *dayName[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char *monthName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
"Jul", "Aug", "Sep", "0ct", "Nov", "Dec"};
char filePath[LENBUFF];
char directoryPath[LENBUFF]; //to store the diretory path like resources
char errPath[512];
char errPath400[512];


int main(int argc, char** argv)
{
    int listenSd, connectSd;
    struct sockaddr_in srvAddr, clntAddr;
    int clntAddrLen, readLen, strLen;
    char rBuff[BUFSIZ];
    int maxFd= 0;
    fd_set defaultFds, rFds;
    int res, i;

           
    //check inputs
    if(argc != 3){
      //printf("Input needed: [Port Number] [Directroy]\n");
       return -1;
    }


    //create socket
    listenSd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSd == -1) errProc("socket");
   
    //address assignment
    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY); //server IP address
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(atoi (argv[1]));
    strcpy(directoryPath, argv[2]);//store directoryPath
    
    //Path to error.html
    snprintf(errPath, sizeof(errPath), "%s/error.html", directoryPath);
    snprintf(errPath400, sizeof(errPath400), "%s/error400.html", directoryPath);

 
    // bind
    res = bind(listenSd, (struct sockaddr *) &srvAddr, sizeof(srvAddr));
    if(res == -1) errProc("bind");
   
    // listen
    res = listen(listenSd, MAXCLNT);
    if(res == -1) errProc("listen");
   
    //FD
    //FD tells you which client is connected. if all bits are 0 => means no one is ready.
    FD_ZERO (&defaultFds);    //initailize 0
    FD_SET(listenSd, &defaultFds);
    maxFd= listenSd;

    while(1)
    {
        //cody the FD
        rFds = defaultFds;
        //select - to find if someone is ready
        //select for I/O multiplexing
        res = select(maxFd + 1, &rFds, 0, 0, NULL);
        if(res == -1) errProc("select");
        for(i = 0;i < maxFd+1; i++)
        {
            // check fd is ready
            if (FD_ISSET(i,&rFds))
            {
                //if it is new client -> connect!
                if(i == listenSd)
                {
                    //printf("client");
                    clntAddrLen = sizeof(clntAddr);
                    //accept
                    connectSd = accept(listenSd,(struct sockaddr*) &clntAddr,&clntAddrLen);
                    if(connectSd == -1) continue;
                    FD_SET(connectSd, &defaultFds);
                    if(maxFd < connectSd) maxFd = connectSd;
                }
                //if it is connected one
                else
                {
                    readLen = read(i,rBuff,BUFSIZ-1);
                    if(readLen == -1) errProc("read");
                    //if client request is finished we can close the connection
                    if(readLen == 0)
                    {
                        FD_CLR(i, &defaultFds);
                        //printf("it is cloesed\n");
                        //If timer works well, is close(i) neccessary?
                        close(i);
                        continue;
                    }
                    //process the message that is in message Buffer
                    processMsg(rBuff,readLen, i);
                }
            }
        }
    }    
}

//processes the received message from the client.
//It takes the received message buffer, its length, and the socket descriptor as input.
int processMsg(char *rBuff, int readLen, int sock)
{
    int counter=0;
    int compare =0;
    int pointer=4;
    int i;
    char lineBuff [LENBUFF];
    char fileTmp [LENBUFF];
    readLineFromMsg(lineBuff,&counter, rBuff);
    //check the request form
    compare=3;
    if(readLen - counter < compare) errProc("processMsg");
    //compare the 3 letters! if it is GET => tranmitReply()
    if( !strncmp(lineBuff, "GET",compare) )
    {
        parseByNullMsg(fileTmp,&pointer,lineBuff);
        fileTmp[pointer-compare-1] = '\0';

        // to Skip processing for favicon.ico (icon shown on the tab, but we don't consider that here.)
        if(strcmp(fileTmp + 1,"favicon.ico") == 0){
            return 0;
        }
        memcpy(filePath, fileTmp+1, pointer-compare-1 );//copy filePath
        transmitReply(sock);
    }
    //if it is POST request
    else if ( !strncmp(lineBuff, "POST", compare+1 ) )
    {
        SendErrMsg(sock);
    }
    //if other than GET or POST
    else{
        SendErrMsg_400(sock);
    }
   
    readLineFromMsg(lineBuff,&counter,rBuff);
}

//This function creates HTTP header. It takes the socket descriptor as input.
//create header file and transimt reply
int transmitReply(int sock)
{
    time_t t;
    FILE *fp;
    int tempSock;
    char fileTmp [LENBUFF];
    char timeVal[30];
    char statusLine[] = "HTTP/1.1 200 Ok\r\n";
    char *generalHeader;
    char responseHeader[] = "Server: Freelec/1.0.0\r\npragma:no-cache\r\ncache-control: private\r\n";
    char entityHeader [] = "Content-Type: text/html; charset-UTF-8\r\n";
   
    //persistent connection: default for http1.1 is keep-alive.
    char connectType[] = "Connection: keep-alive\r\n";
    //timer for 50 seceonds, maximum 100 requests
    char timer[] = "Keep-Alive: timeout=50, max=100\r\n";
    char tmpdirectoryPath[LENBUFF];
    long fileSize;
    strcpy(tmpdirectoryPath, directoryPath);
    struct tm gmtm;
    time(&t);
    gmtime_r(&t,&gmtm);
    strftime(timeVal, 30, "xxx, %d xxx %Y %H:%M:%S GMT", &gmtm);
    memcpy (timeVal, dayName [gmtm.tm_wday],3);
    memcpy (timeVal+8,monthName [gmtm.tm_mon],3);
    generalHeader = malloc(36);
    strncpy (generalHeader, "Date: ",6);
    memcpy (generalHeader+6, timeVal,30);
   
   
    tempSock = dup(sock);
    fp = fdopen(tempSock, "w");
   
    //filePath triming
    //in case of http://localhost:portnumber -> filePath is empty
    if (strcmp(filePath, "") == 0) {
        strcpy(filePath, directoryPath);
        strcat(filePath, "/index.html");
    } //http://localhost:portnumber/resources  
    else if(strcmp(filePath, directoryPath) == 0 ){
        strcat(filePath, "/index.html");
    }
    //http://localhost:portnumber/resources/  
    else if(strcmp(filePath, strcat(tmpdirectoryPath, "/")) == 0 ) {
        strcat(filePath, "index.html");
    }
    //http://localhost:portnumber/   -> request:/
    else if(strcmp(filePath, "/") == 0) {
        strcpy(filePath, directoryPath);
        strcat(filePath, "index.html");
    }
    //http://localhost:portnumber/directory/index.html
    else if (strstr(filePath, directoryPath) == filePath) {
        //if the Path is proper form
        //do nothing
    }
    //filePath like: small.png, script.js    
    // add {directoryPath} in very front (like script.js => resources/script.js)
    else {
        // Create a temporary buffer to store the modified file path
        char tempFilePath[512];
        snprintf(tempFilePath, sizeof(tempFilePath), "%s/%s", directoryPath, filePath);
        // Copy the modified file path back to the original filePath
        strncpy(filePath, tempFilePath, sizeof(filePath) - 1);
        filePath[sizeof(filePath) - 1] = '\0'; // Ensure null-termination
    }
   
    /*Error 404*/
    // Check if the requested file exists and is accessible
    if (access(filePath, F_OK) == -1) {
        fprintf(fp, "HTTP/1.1 404 Not Found\r\n");
        size_t directoryPathLength = strlen(directoryPath);
        const char* errorPage = "/error404.html";
        // Check if the directoryPath and errorPage can fit within the filePath buffer
        if (directoryPathLength + strlen(errorPage) < sizeof(filePath)) {
            strcpy(filePath, directoryPath);
            strcat(filePath, errorPage);
        } else {
            //oops! error in c
            return -1;
        }  
    }
    // Calculate file size to let client know when to finish!
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        // Handle file open error
        fclose(fp);
        free(generalHeader);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fclose(file);
    /* Create the header
    Content-Length is neccessary! Without this, the page loading loops forever...*/
    char contentLengthHeader[64];
    snprintf(contentLengthHeader, sizeof(contentLengthHeader), "Content-Length: %ld\r\n", fileSize);
    fputs(statusLine,fp);
    fwrite(generalHeader ,1,36,fp);
    fputs("\r\n",fp);
    fputs(responseHeader,fp);
    fputs(entityHeader,fp);
    fputs(contentLengthHeader, fp);
    fputs (connectType, fp);
    fputs(timer, fp);
    fputs("\r\n",fp);
    sendFile(fp);
    fflush(fp);
    fclose(fp);
    free(generalHeader);
    return 0;
}

//send file to client
int sendFile(FILE *fp)
{
    FILE *in;
    char buffer[BUFSIZ];
    size_t bytesRead;
    in = fopen(filePath, "r");
    if (in == NULL) errProc("fopen");

    int fd = fileno(fp); // Get the file descriptor associated with the FILE*
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        size_t bytesWritten = fwrite(buffer, 1, bytesRead, fp);
        if (bytesWritten < bytesRead) {
            fclose(in);
            return -1;
        }
    }

    fclose(in);
    return 0;
}



//This function extracts message from the client. returns byte
int readLineFromMsg(char *out, int *point, char* in)
{
    char *start, *ptr;
    int counter;
    ptr =in;
    ptr += *point;
    start = ptr;
    counter=0;
    while(1)
    {
        if(*ptr != 0x0d)
        {
            ptr++;
            counter++;
        }
        else{
            ptr++;
            counter++;
            if(*ptr == 0x0a)break; //read untill \n
        }
    }
    counter++;
    memcpy(out,start,counter);
    return counter;
}

//This function extracts a substring from the input message until a null character ('\0') or space character (' ') is encountered.
int parseByNullMsg(char *out, int *point, char* in)
{
    char *start, *ptr;
    int counter;
    ptr=in;
    ptr += *point;
    start=ptr;
    counter=0;
    while(*ptr != 0x20)
    {
        ptr++;
        counter++;
    }
    memcpy(out,start,counter);
    *point += counter;
    return counter+1;
}

void errProc(const char *str)
{
    //fprintf(stderr, "%s: %s \n",str, strerror(errno));
    exit(errno);
}

//send 405 error when request is POST Method
void SendErrMsg(int sock)
{
    FILE *fp;
    FILE *in;
    int tempSock;
    char statusLine[] = "HTTP/1.1 405 Hethod Not Allowed\r\n";
    char c;
    char responseHeader [] = "Server: Freelec/1.0.0\rinPragma:no-cache\r\ncache-control: private\r\n";
    char entityHeader[] = "Content-Type: text/html; charset-UTF-8\r\n";
    char connectType[] = "Connection: keep-alive\r\n";
    tempSock = dup(sock);
    fp = fdopen(tempSock, "w");
    in = fopen( errPath, "r");
    if(in == NULL) errProc("fopen");
    fputs (statusLine,fp);
    fputs (responseHeader,fp);
    fputs(entityHeader,fp) ;
    fputs(connectType, fp) ;
    fputs("\r\n",fp);
   
    while(1)
    {
        c=fgetc(in);
        if(feof(in)) break;
        fputc(c,fp);
    }
    fflush(fp);
    fclose(in);
    fclose(fp);
    return;
}
//send 400 method when request is other than GET or POST
void SendErrMsg_400(int sock){
    FILE *fp;
    FILE *in;
    int tempSock;
    char statusLine[] = "HTTP/1.1 400 Bad Request\r\n";
    char c;
    char responseHeader [] = "Server: Freelec/1.0.0\rinPragma:no-cache\r\ncache-control: private\r\n";
    char entityHeader[] = "Content-Type: text/html; charset-UTF-8\r\n";
    char connectType[] = "Connection: keep-alive\r\n";
    tempSock = dup(sock);
    fp = fdopen(tempSock, "w");
    in = fopen( errPath400, "r");
    if(in == NULL) errProc("fopen");
    fputs (statusLine,fp);
    fputs (responseHeader,fp);
    fputs(entityHeader,fp) ;
    fputs(connectType, fp) ;
    fputs("\r\n",fp);
    while(1)
    {
        c=fgetc(in);
        if(feof(in)) break;
        fputc(c,fp);
    }
    fflush(fp);
    fclose(in);
    fclose(fp);
    return;
}


