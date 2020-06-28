#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define N 1000
#define KEY_NUM 9527

struct share_m
{
    int count;
    char token[5][20];
};

int main()
{
	char buf[N], token[21];
	int client_socket, buf_size;
	
	struct sockaddr_in6 *serveraddr;
	struct addrinfo *result;
	struct addrinfo hint;

	char *port = "50000";
	char *host = "2001:0:53aa:64c:2c4d:4456:8c6e:5548";

    int shm_id;
    void *shm_addr;
    struct share_m *share;

    shm_id = shmget((key_t)KEY_NUM, sizeof(struct share_m), IPC_CREAT|0666);
    shm_addr = shmat(shm_id, (void*)0, 0);
    share = (struct share_m *)shm_addr;

	if ((client_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
	{
		printf("Can't create a socket\n");
		return 1;
	}
  
	memset(&hint, 0x00, sizeof(hint));
	hint.ai_family = AF_INET6;
	hint.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hint, &result) != 0)
	{
		printf("Can't get server's address\n");
		return 1;
	}
    
	serveraddr = (void*)result->ai_addr;
	
 	if (connect(client_socket, (struct sockaddr*)serveraddr, sizeof(*serveraddr)) < 0)
	{
		printf("Can't connect\n");
		return 1;
	}
    printf("Connected\n");

	while ((buf_size = read(0, buf, N)) > 0)
	{
        buf[buf_size-1] = 0x0a;
		write(client_socket, buf, buf_size);
		printf("send : %s\n", buf);
        
        if (strncmp(buf, "OK", 2) == 0)
        {
            for (int i = 0; i < N; i++)
		        buf[i] = 0;
            read(client_socket, buf, N); 
            printf("recv : %s\n", buf);

            printf("waiting token..\n");
            sleep(3);
            
            memset(token, 0, 21);
            for (int i = 0; i < share->count; i++)
            {
                strncpy(token, share->token[i], 20);
                token[20] = 0x0a;
                write(client_socket, token, 21);
                printf("send : %s\n", token);
            }
        }
        
        for (int i = 0; i < N; i++)
		    buf[i] = 0;

        read(client_socket, buf, N); 
        printf("recv : %s\n", buf);
		for (int i = 0; i < N; i++)
			buf[i] = 0;
	}

	close(client_socket);
	return 0;
}
