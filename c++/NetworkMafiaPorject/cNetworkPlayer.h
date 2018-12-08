#pragma once
#include "stdafx.h"

enum ePlayerJob
{
	E_PERSON = 0,
	E_MAFIA = 1,
	E_POLICE = 2,
	E_JOBLENGTH = 3
};

enum ePlayerState
{
	E_DEAD,
	E_ALIVE,
};

class cNetworkPlayer
{
	ePlayerJob m_PlayerJob;
	ePlayerState m_PlayerState;
	std::string m_PlayerName;


	class iPlayerDelegate* m_Delegate;

	SOCKET m_PlayerSocket;
	
	char m_RecvBuf[SIZE_BUF];
	char m_SendBuf[SIZE_BUF];
	size_t m_SendBufSize = 0;

	clock_t m_LastPing;
	std::mutex m_PlayerMutex;
	HANDLE threadHandle;
	DWORD m_ThreadID;
public:
	cNetworkPlayer();
	~cNetworkPlayer();

	static DWORD WINAPI ThreadFunc(void* player);

	void Start(SOCKET& sock);
	void RunThread();

	void ChangeToData();
	void SendMessageToClient(std::string message);

	class iPlayerDelegate* Delegate() const { return m_Delegate; }
	void Delegate(class iPlayerDelegate* val) { m_Delegate = val; }


	std::string PlayerName() { return m_PlayerName; }
	void PlayerName(std::string playerName) 
	{ 
		m_PlayerName = playerName; 
	}
	
	ePlayerState PlayerState() { return m_PlayerState; }
	void PlayerState(ePlayerState playerState)
	{
		m_PlayerState = playerState;
	}

	ePlayerJob PlayerJob() { return m_PlayerJob; }
	void PlayerJob(ePlayerJob playerJob)
	{
		m_PlayerJob = playerJob;
	}

};

