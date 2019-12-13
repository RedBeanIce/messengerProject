#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define IP "127.0.0.1"
#define MAX_NAME    30
#define MAX_MSG    1024

typedef struct client{	//클라이언트 구조체 
    int disc;
    int unum;
    int num;
    char client_name[MAX_NAME];
}Client;

int service;
int num;
int flag = 0;
pthread_mutex_t user_mutex;
char file_name[100];	//파일 이름을 저장할 변수 

void cSocket_Exit(int signo);
int socketss(char *ip, int port);
void *send_message(void *client);
void *receive_message(void *client);
void chating_start(int dsc);

void chating_start(int dsc)	//채팅방에 입장시 시작 
{
    Client client;
    pthread_t ptr[2];
    char name[MAX_NAME]="";	//이름을 저장할 변수 
    char rbuf[MAX_MSG]="";	//메세지를 저장할 변수 

    recv(dsc, rbuf, 100, 0);	//서버에게 받은 메시지를 rbuf에 저장, 이름을 입력해달라는 문구 
    fputs(rbuf,stdout);		//출력 
    fgets(name, sizeof(name), stdin);	//이름을 입력받음 
    send(dsc, name, strlen(name), 0);	//입력받은 이름을 서버에 전송 
    client.disc = dsc;
    strcmp(client.client_name,name);
    
    pthread_create(&ptr[0], NULL, receive_message, &client);	//메세지를 수신할 쓰레드 생성 
    pthread_detach(ptr[0]);
    pthread_create(&ptr[1], NULL, send_message, &client);	//메세지를 송신할 쓰레드 생성 
    pthread_detach(ptr[1]);

    while(1)
        pause();
}

void *receive_message(void *client)	//메세지를 수신받는 함수 
{
    Client cs = *(Client *)client;
    char rbuf[MAX_MSG];	//메세지를 저장할 변수 
    int fd;	//파일을 저장할 때 사용할 변수 
    int cnt;	//파일을 저장할 때 사용할 변수 

    memset(rbuf, 0, sizeof(rbuf));	//버퍼 초기화 

    while(1)
    {
        if(flag == 0)	//한번만 실행 
        {
            recv(cs.disc, rbuf, sizeof(rbuf), 0);
            memset(rbuf, 0, sizeof(rbuf));	//버퍼초기화 
        }

        while(recv(cs.disc, rbuf, sizeof(rbuf), 0) > 0)
        {
            if(flag == 1)	//파일 전송을 요청했을 때 실행 
            {
                fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);	//요청한 파일명으로 파일 생성 
                cnt = write(fd, rbuf, strlen(rbuf));	//생성한 파일에 수신한 파일을 작성 
                close(fd);
                flag = 0;
       			memset(rbuf, 0, sizeof(rbuf));	//버퍼초기화 
            }
            else
            {
                fputs(rbuf, stdout);	//출력 
                memset(rbuf, 0, sizeof(rbuf));	//버퍼초기화 
            }
        }
    }
}

void *send_message(void *client)	//메세지를 송신하는 함수 
{
    Client cs = *(Client *)client;
    char sbuf[MAX_MSG];	//메세지를 저장할 변수 
    char *fio;	//파일명을 분리하기 위한 변수 

    while(1)
    {
        fgets(sbuf, sizeof(sbuf), stdin);	//전송할 메세지를 입력받음 
        send(cs.disc, sbuf, sizeof(sbuf), 0);	//메세지를 전송 
        if(!strncmp(sbuf, "/f", 2)){	//파일 전송을 요청하는 경우 실행 
            flag = 1;	//receive_message 함수에서 파일 전송 메세지를 구분하기 위함 
            fio = strtok(sbuf, " ");	//앞에 '/f'를 자름 
            fio = strtok(NULL, " ");	//파일명을 자름 
            fio[strlen(fio)-1] = '\0';	//fgets는 마지막에 줄바꿈 문자까지 입력받으므로 대신'\0'를 삽입 
            memset(file_name, 0, sizeof(file_name));	//파일명을 저장할 변수 초기화 
            strcpy(file_name, fio);	//자른 파일명을 file_name 변수에 복사 
        }
        memset(sbuf, 0, sizeof(sbuf));	//버퍼 초기화 
    }
}

int socketss(char *ip, int port)	//서버와 연결시 사용 
{
    int dsc;
    
    //IPv4 주소영역의 양방향 TCP/IP 기반의 통신 
    if((dsc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {	//생성 실패시 메시지 
        perror("sock error");
        return -1;
    }
    
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;	//IPv4 주소체계
    servaddr.sin_addr.s_addr = inet_addr(ip);	//IP주소 
    servaddr.sin_port = htons(port);	//포트번호 

    if(connect(dsc, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {	//연결 실패시 메시지 
        perror("connect");
        return -1;
    }

    return dsc;
}

void cSocket_Exit(int signo)	//종료시 사용 
{
    printf("클라이언트를 종료합니다.\n");
    close(service);
    exit(0);
}

int main(int argc,char *argv[])
{
    int port;
    char pt[MAX_MSG];

    printf("포트번호를 입력하세요 :");
    scanf("%d", &port);	//포트 번호를 입력받음
    getchar();
    signal(SIGINT, cSocket_Exit);
    
    pthread_mutex_init(&user_mutex, NULL);
    
    if((service = socketss(IP, port)) == -1)	//서버와 연결 
    {	//실패시 메시지 
        perror("socket");
        return 0;
    }

    chating_start(service);	//채팅방 입장 

    return 0;
}


