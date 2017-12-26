#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
int sockfd;                  //套接字
struct sockaddr_in servaddr; //服务器
char buff[1024];
int ret;         //select函数返回值
fd_set rdfds;    //文件描述符集合
FD_ZERO(&rdfds); //清空集合
int stdinfd = 0;
void getTime()
{ //该函数用于打印当前时分秒
    time_t tmpcal_ptr;
    struct tm *tmp_ptr = NULL;
    time(&tmpcal_ptr);
    tmp_ptr = localtime(&tmpcal_ptr);
    printf("%d:%d:%d", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);
}
int conn()
{
    //建立套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 1;




        
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                            //协议
    servaddr.sin_port = htons(atoi(argv[2]));                 //端口
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) //地址
    {                                                         //将参数1转为IP地址
        struct hostent *hptr;                                 //通过域名获取ip
        hptr = gethostbyname(argv[1]);
        if (hptr == NULL)
        {
            printf("ip address or domin error for %s\n", argv[1]);
            return 1;
        }
        servaddr.sin_addr = *(struct in_addr *)hptr->h_addr;
    }
    //建立连接
    if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        getTime();
        printf("  connect error: %s(errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    getTime();
    printf("Please enter the keyword!\n");
    memset(buff, 0, sizeof(buff));
    scanf("%s", buff);
    if (send(sockfd, buff, strlen(buff), 0) == -1)
    {
        getTime();
        printf("  sent keyword error: %s(errno: %d)\n", strerror(errno), errno);
    }
    memset(buff, 0, sizeof(buff));
    int recvbytes = recv(sockfd, buff, sizeof(buff), 0);
    if (recvbytes == -1)
    {
        getTime();
        printf("  recv message error: %s (errno: %d)\n", strerror(errno), errno);
        return 1;
    }
    else if (recvbytes == 0)
        return 1;
    else
    {
        if (strcmp(buff, "RIGHT") != 0)
        {
            getTime();
            printf("  keyword error!\n");
            return 1;
        }
    }
    return 0;
}
int main(int argc, char *argv[])
{ //主函数
    //判断参数个数
    if (argc != 3)
    {
        printf("usage: ./client <ipaddress|domin> <portnumber>\n");
        return 1;
    }
    if (conn() == 0)
    {//建立连接
        getTime();
        printf("  successfully connected!\n");
    }
    else
        return 1;

    while (1)
    {
        FD_SET(stdinfd, &rdfds);
        FD_SET(sockfd, &rdfds); //把socketfd加入集合
        printf("===========waiting for server's message===========\n");
        ret = select(sockfd + 1, &rdfds, NULL, NULL, NULL);
        if (ret <= 0)
            printf("error!\n");
        else
        {
            if (FD_ISSET(stdinfd, &rdfds))
            { //如果有标准输入
                memset(buff, 0, sizeof(buff));
                fgets(buff, 1024, stdin);
                buff[strlen(buff) - 1] = '\0';
                if (send(sockfd, buff, strlen(buff), 0) == -1)
                {
                    getTime();
                    printf("  sent message error: %s(errno: %d)\n", strerror(errno), errno);
                    break;
                }
                else
                    printf("message sent.\n");
            }
            if (FD_ISSET(sockfd, &rdfds))
            { //如果有socket输入
                memset(buff, 0, sizeof(buff));
                int recvbytes = recv(sockfd, buff, sizeof(buff), 0);
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
    close(sockfd);
    return 0;
}
