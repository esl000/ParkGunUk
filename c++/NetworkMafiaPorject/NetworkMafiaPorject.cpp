// NetworkMafiaPorject.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "cMafiaGame.h"



void ErrorHandling(const char *message);
DWORD WINAPI MainThreadFunc(void* game);

std::mutex g_MainMutex;

int main(int argc, char *argv[])
{
	DWORD mainThreadID;
	HANDLE mainThreadHandle = CreateThread(NULL, 0, MainThreadFunc, (void*)g_Game, 0, &mainThreadID);

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	int strLen, i;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");


	//멀티 쓰레드 이용
	//연결 대기
	if (listen(hServSock, 20) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	clntAdrSize = sizeof(clntAdr);
	for (i = 0; i < 20; i++)
	{
		//연결수락
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr,
			& clntAdrSize);

		g_Game->ConnectSocket(hClntSock);
	}

	CloseHandle(mainThreadHandle);

	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

DWORD WINAPI MainThreadFunc(void * game)
{
	g_Game->GameStart();
	return 0;
}

