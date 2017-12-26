#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXN 10

int main(int argc, char* argv[])
{
    int listenfd;
    int connfd = 0;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    char ip_addr[16];
    socklen_t clientaddr_len;
    char buff[1024];
    int flag=1;
       
    if(argc<2) {
        printf("usage: ./server <portnum>\n");
        return 1;
    }
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s (errno: %d)\n",strerror(errno),errno);
        return 1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        printf("set socket option error: %s (errno: %d)\n",strerror(errno),errno);
        return 1;
    }
    if(bind(listenfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s (errno: %d)\n",strerror(errno),errno);
        return 1;
    }
    if(listen(listenfd, MAXN) == -1) {
        printf("listen socket error: %s (errno: %d)\n",strerror(errno),errno);
        return 1;
    }
    
    printf("===========waiting for client's connect===========\n");
    memset(&clientaddr, 0, sizeof(clientaddr));
    if((connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddr_len)) == -1) {
        printf("accept socket error: %s (errno: %d)\n",strerror(errno),errno);
        return 1;
    }
    inet_ntop(AF_INET, &clientaddr.sin_addr, ip_addr, 16);
    printf("client IP:%s, port:%d, connected\n", ip_addr, ntohs(clientaddr.sin_port));
    
    while(1) {
        printf("===========waiting for client's message===========\n");
        memset(buff, 0, sizeof(buff));
        int recvbytes = recv(connfd, buff, sizeof(buff), 0);
        if(recvbytes == -1) {
            printf("recv message error: %s (errno: %d)\n",strerror(errno),errno);
            break;
        }
        else if(recvbytes == 0) break;
        else printf("recv message: %s\n",buff);
        
        printf("===========type message sent to client===========\n");
        memset(buff, 0, sizeof(buff));
        scanf("%s", buff);
        if(send(connfd, buff, strlen(buff), 0) == -1) {
            printf("sent message error: %s(errno: %d)\n",strerror(errno),errno);
            break;
        }
        else printf("message sent.\n");
    }
    close(connfd);
    close(listenfd);
    return 0;
}
