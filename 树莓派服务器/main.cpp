#include<WinSock2.h>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXN 10
#include<WS2tcpip.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
SOCKET client1;
SOCKET client2;
void getTime()
{
	struct tm tmp_ptr;
	time_t now=time(NULL);
	localtime_s(&tmp_ptr,&now);
	printf("%d:%d:%d", tmp_ptr.tm_hour, tmp_ptr.tm_min, tmp_ptr.tm_sec);
}
DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	SOCKET client = (SOCKET)lpParameter;//发送客户端
	SOCKET csend;
	if (client == client1)//接收客户端
		csend = client2;
	else
		csend = client1;
	char buff[1024];
	while (true)
	{
		memset(buff, 0, sizeof(buff));
		int recvbytes = recv(client, buff, sizeof(buff), 0);

		if (recvbytes == -1)
		{
			getTime();
			printf("  recv message error: %d\n", WSAGetLastError());
			break;
		}
		else if (recvbytes == 0) {
			getTime();
			cout << "  client disconnect" << endl;
			closesocket(csend);
			break;
		}
		else
		{
			getTime();
			printf("  recv message: %s\n", buff);
			if (send(csend, buff, recvbytes, 0) !=recvbytes)
			{
				getTime();
				printf("  sent message error: %d\n", WSAGetLastError());
				break;
			}
			else {
				getTime();
				printf("  message sent from.\n");
			}

		}

	}

	return 0;
}

int main()
{
	WSADATA wsaData;
	HANDLE hThread1 = NULL;
	HANDLE hThread2 = NULL;
	SOCKET server;
	SOCKADDR_IN servaddr;   //服务器地址
	SOCKADDR_IN client1addr; //客户端1地址
	SOCKADDR_IN client2addr; //客户端2地址
	char ip_addr[16];              //ip地址
	int clientaddr_len = sizeof(SOCKADDR);
	char flag = 1;
	char buff[1024];

	int port;
	char keyword[128];
	cout << "Please enter the Port" << endl;
	cin >> port;
	cout << "Please enter the keyword" << endl;
	cin >> keyword;
	/*if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("create socket error: %s (errno: %d)\n", strerror(errno), errno);
		return 1;
	}*/
	// 加载套接字
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("create socket error：%d......\n", WSAGetLastError());
		return 1;
	}
	server = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;                //指定网络协议IPV4
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //设置服务器IP地址
	servaddr.sin_port = htons(port);     //设置端口号
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1)
	{
		printf("set socket option error: %d......\n", WSAGetLastError());
		return 1;
	}
	if (bind(server, (LPSOCKADDR)&servaddr, sizeof(servaddr)) == -1)
	{
		printf("bind socket error：%d......\n", WSAGetLastError());
		return 1; //尝试绑定端口
	}
	if (listen(server, MAXN) == -1)
	{
		printf("listen socket error：%d......\n", WSAGetLastError());
		return 1; //尝试监听
	}
	//等待客户端1连接
	printf("===========waiting for client1's connect===========\n");
	memset(&client1addr, 0, sizeof(client1addr));
	if ((client1 = accept(server, (SOCKADDR *)&client1addr, &clientaddr_len)) == -1)
	{
		getTime();
		//printf("  accept socket error: %s (errno: %d)\n", strerror(errno), errno);
		printf("  accept socket error：%d......\n", WSAGetLastError());
		return 1;
	}


	memset(buff, 0, sizeof(buff));
	int recvbytes = recv(client1, buff, sizeof(buff), 0);
	if (recvbytes == -1)
	{
		getTime();
		printf("  recv keyword error: %d", WSAGetLastError());
		return 1;
	}
	else if (recvbytes == 0)
		return 1;
	else
	{
		if (strcmp(buff, keyword) != 0)
		{//密码错误
			getTime();
			printf("  keyword error!\n");
			memset(buff, 0, sizeof(buff));
			strcpy(buff, "ERROR");
			if (send(client1, buff, strlen(buff), 0) == -1)
			{
				getTime();
				printf("  sent message error: %d", WSAGetLastError());
			}
			return 1;
		}
		else
		{//密码正确
			memset(buff, 0, sizeof(buff));
			strcpy(buff, "RIGHT");
			if (send(client1, buff, strlen(buff), 0) == -1)
			{
				getTime();
				printf("  sent message error: %d", WSAGetLastError());
			}
		}
	}
	
	inet_ntop(AF_INET, &client1addr.sin_addr, ip_addr, 16); //将IP地址转化为字符串
	getTime();
	printf("  client IP:%s, port:%d, connected\n", ip_addr, ntohs(client1addr.sin_port));
	//等待客户端2连接
	printf("===========waiting for client2's connect===========\n");
	memset(&client2addr, 0, sizeof(client2addr));
	if ((client2 = accept(server, (SOCKADDR *)&client2addr, &clientaddr_len)) == -1)
	{
		getTime();
		//printf("  accept socket error: %s (errno: %d)\n", strerror(errno), errno);
		printf("  accept socket error：%d......\n", WSAGetLastError());
		return 1;
	}


	memset(buff, 0, sizeof(buff));
	recvbytes = recv(client2, buff, sizeof(buff), 0);
	if (recvbytes == -1)
	{
		getTime();
		printf("  recv keyword error: %d", WSAGetLastError());
		return 1;
	}
	else if (recvbytes == 0)
		return 1;
	else
	{
		if (strcmp(buff, keyword) != 0)
		{//密码错误
			getTime();
			printf("  keyword error!\n");
			memset(buff, 0, sizeof(buff));
			strcpy(buff, "ERROR");
			if (send(client2, buff, strlen(buff), 0) == -1)
			{
				getTime();
				printf("  sent message error: %d", WSAGetLastError());
			}
			return 1;
		}
		else
		{//密码正确
			memset(buff, 0, sizeof(buff));
			strcpy(buff, "RIGHT");
			if (send(client2, buff, strlen(buff), 0) == -1)
			{
				getTime();
				printf("  sent message error: %d", WSAGetLastError());
			}
		}
	}


	inet_ntop(AF_INET, &client2addr.sin_addr, ip_addr, 16); //将IP地址转化为字符串
	getTime();
	printf("  client IP:%s, port:%d, connected\n", ip_addr, ntohs(client2addr.sin_port));

	printf("===========waiting for client's message===========\n");
	hThread1 = CreateThread(NULL, 0, ClientThread, (LPVOID)client1, 0, NULL);
	if (hThread1 == NULL)
	{
		cout << "Create Thread Failed!" << endl;
	}
	hThread2 = CreateThread(NULL, 0, ClientThread, (LPVOID)client2, 0, NULL);
	if (hThread2 == NULL)
	{
		cout << "Create Thread Failed!" << endl;
	}
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	closesocket(server);
	closesocket(client1);
	closesocket(client2);
	WSACleanup();
	system("pause");
	return 0;
}
