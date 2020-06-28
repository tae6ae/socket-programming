#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define N 1000
#define KEY_NUM 9527

struct share_m
{
    int count;
    char token[5][20];
};

int main()
{
    char buf[N];
    int listen_socket, server_socket;
    int clilen, recv_size, child, count;

    struct sockaddr_in serveraddr, clientaddr;

    int port = 36500;

    int shm_id;
    void *shm_addr;
    struct share_m *share;

    pid_t pid;

    shm_id = shmget((key_t)KEY_NUM, sizeof(struct share_m), IPC_CREAT|0666);
    shm_addr = shmat(shm_id, (void*)0, 0);
    share = (struct share_m *)shm_addr;

    if ((listen_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Can't create a socket\n");
        return 1;
    }

    memset(&serveraddr, 0x00, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        printf("Can't bind name to socket\n");
        return 1;
    }

    listen(listen_socket, 5);
    child = 0;
    count = 0;
    share->count = 0;
    while (1)
    { 
        clilen = sizeof(clientaddr);
        if ((server_socket = accept(listen_socket, (struct sockaddr*)&clientaddr, &clilen)) < 0)
        {
            printf("Can't accept client\n");
            return 1;
        }

        if ((pid = fork()) == 0)
        {
            close(listen_socket);
            printf("connect\n");
            memset(buf, 0x00, N);
            recv_size = read(server_socket, buf, N);
            printf("recv : %s", buf);
            printf("disconnect\n");
            strncpy(share->token[count], buf, 20);
            share->count++;
            exit(0);
        }

        close(server_socket);
        child++;
        count++;

        while (child)
        {
            pid = waitpid((pid_t)-1, NULL, WNOHANG); 
            if (pid == 0)
                break;
            else
                child--;
        }
    }

    close(listen_socket);
    return 0;
}
