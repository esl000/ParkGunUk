// ClientMafia.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"


#define BUF_SIZE 1024
void ErrorHandling(const char *message);
DWORD WINAPI InputThread(void* args);
//DWORD WINAPI SocketThread(void* args);

//DWORD WINAPI SocketThread(void* args)
//{
//	char recvData[SIZE_BUF];
//	int strLen;
//
//	while (true)
//	{
//		mainMutex.lock();
//		send(hSocket, sendMessage, strlen(sendMessage), 0);
//		memset(sendMessage, 0, SIZE_BUF);
//		mainMutex.unlock();
//
//		strLen = recv(hSocket, recvData, BUF_SIZE - 1, 0);
//		recvData[strLen] = 0;
//
//		if (recvData[0] == 0)
//			continue;
//
//		mainMutex.lock();
//		memcpy(recvMessage, recvData, SIZE_BUF);
//		std::cout << recvData;
//		memset(recvMessage, 0, SIZE_BUF);
//		mainMutex.unlock();
//	}
//}

SOCKET hSocket;
char sendMessage[BUF_SIZE];
char recvMessage[BUF_SIZE];
std::mutex mainMutex;
bool gameEnd = false;

DWORD WINAPI InputThread(void* args)
{
	char inputMessage[SIZE_BUF];

	while (true)
	{
		fgets(inputMessage, BUF_SIZE, stdin);
		inputMessage[strlen(inputMessage) - 1] = 0;

		//if ((strcmp(inputMessage, "q\n") || strcmp(inputMessage, "Q\n")))
		//	break;


		mainMutex.lock();
		memcpy(sendMessage, inputMessage, SIZE_BUF);
		mainMutex.unlock();
	}

	gameEnd = true;
	return 0;
}

int main(int argc, char *argv[])
{
	char name[20];
	std::cout << "이름 : ";
	std::cin >> name;

	//int cutName = strlen(name);
	//name[cutName - 2] = 0;
	//name[cutName - 1] = 0;

	std::string nameSettingCommand("-playersetting " + std::string(name));
	
	memcpy(sendMessage, &nameSettingCommand[0], nameSettingCommand.size());


	WSADATA wsaData;
	int strLen;
	SOCKADDR_IN servAdr;

	HANDLE inputThread;

	//if (argc != 3) {
	//	printf("Usage : %s <IP> <port>\n", argv[0]);
	//	exit(1);
	//}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(atoi("9090"));
	if (connect(hSocket, (SOCKADDR*)&servAdr,
		sizeof(servAdr)) == SOCKET_ERROR)

		ErrorHandling("connect() error!");
	else
		puts("Connected...........");

	DWORD inputThreadID;
	inputThread = CreateThread(NULL, 0, InputThread, NULL, 0, &inputThreadID);

	char recvData[SIZE_BUF];
	char sendData[SIZE_BUF];

	while (!gameEnd)
	{
		mainMutex.lock();
		memcpy(sendData, sendMessage, SIZE_BUF);
		memset(sendMessage, 0, SIZE_BUF);
		mainMutex.unlock();

		send(hSocket, sendData, BUF_SIZE, 0);

		strLen = recv(hSocket, recvData, BUF_SIZE, 0);
		recvData[strLen-1] = 0;

		if (recvData[0] == 0)
			continue;

		mainMutex.lock();
		memcpy(recvMessage, recvData, SIZE_BUF);
		std::cout << recvData;
		memset(recvData, 0, SIZE_BUF);
		mainMutex.unlock();
	}

	CloseHandle(inputThread);

	//while (1)
	//{
	//	fputs("Input message(Q to quit): ", stdout);
	//	fgets(message, BUF_SIZE, stdin);
	//	if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
	//		break;

	//		send(hSocket, message, strlen(message), 0);
	//	strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
	//	message[strLen] = 0;
	//	printf("Message from server: %s", message);
	//}
	closesocket(hSocket);
	WSACleanup();
	return 0;
}
void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

