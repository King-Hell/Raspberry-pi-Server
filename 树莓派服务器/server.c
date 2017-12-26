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
#include <time.h>
void getTime()
{
    time_t tmpcal_ptr;
    struct tm *tmp_ptr = NULL;
    time(&tmpcal_ptr);
    tmp_ptr = localtime(&tmpcal_ptr);
    printf("%d:%d:%d", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);
}

int main(int argc, char *argv[])
{
    int listenfd;
    int connfd = 0;
    struct sockaddr_in servaddr;   //服务器地址
    struct sockaddr_in clientaddr; //客户端地址
    char ip_addr[16];              //ip地址
    socklen_t clientaddr_len;
    char buff[1024];
    int flag = 1;

    int ret;         //select返回值
    fd_set rdfds;    //fd集合
    FD_ZERO(&rdfds); //清空集合
    int stdinfd = 0;

    if (argc < 3)
    {
        printf("usage: ./server <portnum> <keyword>\n");
        return 1;
    }
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                //指定网络协议IPV4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //设置服务器IP地址
    servaddr.sin_port = htons(atoi(argv[1]));     //设置端口号
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1)
    {
        printf("set socket option error: %s (errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    if (bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 1; //尝试绑定端口
    }
    if (listen(listenfd, MAXN) == -1)
    {
        printf("listen socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 1; //开始监听
    }

    printf("===========waiting for client's connect===========\n");
    memset(&clientaddr, 0, sizeof(clientaddr));
    if ((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddr_len)) == -1)
    {
        getTime();
        printf("  accept socket error: %s (errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    memset(buff, 0, sizeof(buff));
    int recvbytes = recv(connfd, buff, sizeof(buff), 0);
    if (recvbytes == -1)
    {
        getTime();
        printf("  recv keyword error: %s (errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    else if (recvbytes == 0)
        return 1;
    else
    {
        if (strcmp(buff, argv[2]) != 0)
        {//密码错误
            getTime();
            printf("  keyword error!\n");
            memset(buff, 0, sizeof(buff));
            strcpy(buff,"ERROR");
            if (send(connfd, buff, strlen(buff), 0) == -1)
            {
                getTime();
                printf("  sent message error: %s(errno: %d)\n", strerror(errno), errno);
            }
            return 1;
        }
        else
        {//密码正确
            memset(buff, 0, sizeof(buff));
            strcpy(buff,"RIGHT");
            if (send(connfd, buff, strlen(buff), 0) == -1)
            {
                getTime();
                printf("  sent message error: %s(errno: %d)\n", strerror(errno), errno);
            }
        }
    }
    inet_ntop(AF_INET, &clientaddr.sin_addr, ip_addr, 16); //将IP地址转化为字符串
    getTime();
    printf("  client IP:%s, port:%d, connected\n", ip_addr, ntohs(clientaddr.sin_port));

    while (1)
    {
        FD_SET(stdinfd, &rdfds);
        FD_SET(connfd, &rdfds); //把connfd加入集合
        printf("===========waiting for client's message===========\n");
        ret = select(connfd + 1, &rdfds, NULL, NULL, NULL);

        if (ret <= 0)
            printf("error!\n");
        else
        {
            if (FD_ISSET(stdinfd, &rdfds))
            {
                //printf("===========type message sent to client===========\n");
                memset(buff, 0, sizeof(buff));
                fgets(buff, 1024, stdin);
                buff[strlen(buff) - 1] = '\0';
                if (send(connfd, buff, strlen(buff), 0) == -1)
                {
                    getTime();
                    printf("  sent message error: %s(errno: %d)\n", strerror(errno), errno);
                    break;
                }
                else
                    printf("message sent.\n");
            }
            if (FD_ISSET(connfd, &rdfds))
            {
                memset(buff, 0, sizeof(buff));
                int recvbytes = recv(connfd, buff, sizeof(buff), 0);
                if (recvbytes == -1)
                {
                    getTime();
                    printf("  recv message error: %s (errno: %d)\n", strerror(errno), errno);
                    break;
                }
                else if (recvbytes == 0)
                    break;
                else
                {
                    getTime();
                    printf("  recv message: %s\n", buff);
                }
            }
        }
    }
    close(connfd);
    close(listenfd);
    return 0;
}
