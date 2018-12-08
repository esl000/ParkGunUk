#include "stdafx.h"
#include "cNetworkPlayer.h"
#include "cMafiaGame.h"


static ePlayerAction Command(std::string& commandStr)
{
	static std::string commandArray[ePlayerAction::E_ACTIONLENGTH] = { "vote", "kill", "check", "playersetting" };
	for (size_t i = 0; i < ePlayerAction::E_ACTIONLENGTH; ++i)
	{
		if (!strcmp(commandArray[i].data(), commandStr.data()))
			return (ePlayerAction)i;
		//commandArray[i].compare(commandStr))
	}
	return ePlayerAction::E_VOTE;
}


cNetworkPlayer::cNetworkPlayer()
	:m_PlayerState(ePlayerState::E_ALIVE)
{
}


cNetworkPlayer::~cNetworkPlayer()
{
	//m_PlayerThread->join();
	//thread	std::thread::native_handle_type threadHandle = m_PlayerThread->native_handle();

	CloseHandle(threadHandle);
}

DWORD cNetworkPlayer::ThreadFunc(void * player)
{
	((cNetworkPlayer*)player)->RunThread();
	return 0;
}

void cNetworkPlayer::Start(SOCKET& sock)
{
	m_PlayerSocket = sock;
	threadHandle = CreateThread(NULL, 0, cNetworkPlayer::ThreadFunc, (void*)this, 0, &m_ThreadID);
}

void cNetworkPlayer::RunThread()
{
	char recvBuf[SIZE_BUF];
	char sendBuf[SIZE_BUF];
	while (true)
	{
		recv(m_PlayerSocket, recvBuf, SIZE_BUF, 0);

		m_PlayerMutex.lock();
		memcpy(m_RecvBuf, recvBuf, SIZE_BUF);
		m_PlayerMutex.unlock();

		//m_PlayerMutex.lock();
		m_LastPing = clock();
		//m_PlayerMutex.unlock();

		ChangeToData();

		m_PlayerMutex.lock();
		memcpy(sendBuf, m_SendBuf, SIZE_BUF);
		memset(m_SendBuf, 0, SIZE_BUF);
		m_SendBufSize = 0;
		m_PlayerMutex.unlock();


		send(m_PlayerSocket, sendBuf, SIZE_BUF, 0);

		//m_PlayerMutex.lock();
		//m_PlayerMutex.unlock();
	}
}

// game main thread에서 처리 할 지 playersocket thread에서 처리할지 고민해보기
void cNetworkPlayer::ChangeToData()
{
	char tempData[SIZE_BUF];
	m_PlayerMutex.lock();
	memset(tempData, 0, SIZE_BUF);
	memcpy(tempData, m_RecvBuf, SIZE_BUF);
	m_PlayerMutex.unlock();
	std::string recvData = tempData;
	size_t tokenIndex = recvData.find('-');

	if (tempData[0] == 0)
	{
		//std::cout << "player sendPing" << std::endl;
		return;
	}

	std::cout << "player sendmessage : " << tempData << std::endl;

	if (tokenIndex != std::string::npos)
	{
		//command
		recvData = &recvData[tokenIndex + 1];


		std::string command;
		size_t tokenIndex = recvData.find(' ');
		if (tokenIndex != std::string::npos && tokenIndex != 0)
		{
			command.resize(tokenIndex + 1, 0);
			memcpy(&command[0], &recvData[0], tokenIndex);
			recvData = &recvData[tokenIndex];

			std::string target;
			size_t tokenIndex = recvData.find(' ');
			if (tokenIndex != std::string::npos)
			{
				target.resize(recvData.size() - 1, 0);
				memcpy(&target[0], &recvData[1], recvData.size() - 1);
				//recvData = &recvData[tokenIndex + 1];
			}

			m_Delegate->InputAction(this, Command(command), target);
		}
	}
	else
	{
		m_Delegate->AddMessage(this, recvData);
	}
}

void cNetworkPlayer::SendMessageToClient(std::string message)
{
	m_PlayerMutex.lock();
	if (message.size() < SIZE_BUF - m_SendBufSize)
	{
		memcpy(&m_SendBuf[m_SendBufSize], message.data(), message.size());
		m_SendBufSize += message.size();
	}
	m_PlayerMutex.unlock();
}
