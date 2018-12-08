#pragma once
#include "stdafx.h"


#define g_Game cMafiaGame::GetInstance()

enum ePlayerAction
{
	E_VOTE = 0,
	E_KILL = 1,
	E_CHECKJOB = 2,
	E_PLAYERSETTING = 3,
	E_ACTIONLENGTH = 4
};

enum eGameState
{
	E_LOBBY,
	E_DAY,
	E_NIGHT,
	E_VOTETIME
};

enum eGameWinType
{
	E_NOTEND = 0,
	E_PERSONWIN = 1,
	E_MAFIAWIN = 2
};

class iPlayerDelegate
{
public:

	virtual void RegisterGame(class cNetworkPlayer* player) = 0;
	virtual void UnRegisterGame(cNetworkPlayer* player) = 0;
	virtual void AddMessage(cNetworkPlayer* player, std::string message) = 0;
	virtual void InputAction(cNetworkPlayer* player, ePlayerAction actionState, std::string targetName) = 0;
};


class cMafiaGame : public iPlayerDelegate
{
	cMafiaGame();

	std::set<class cNetworkPlayer*> m_vecPlayer;
	std::map<std::string, cNetworkPlayer*> m_vecPlayerByCharacter;
	eGameState m_GameState;
	eGameWinType m_CurrentGameWin;

	std::set<cNetworkPlayer*> m_votePlayer;
	std::map<cNetworkPlayer*, int> m_voteTargetPlayer;
	cNetworkPlayer* m_mafiaKilledPlayer;
	bool m_bCheckedPlayer;
	bool m_bRenderCheckedPlayerJob;
	cNetworkPlayer* m_checkedPlayer;

public:
	~cMafiaGame();

	static cMafiaGame* GetInstance();

	void GameStart();
	void ConnectSocket(SOCKET& clientSocket);
	void SendSystemMessage(cNetworkPlayer* player, std::string message);
	void SendSystemMessage(std::string message);
	void SetPlayerDie(cNetworkPlayer* player);
	void JobSetting();
	eGameWinType CheckGameWin();
	bool isGameEnd(eGameWinType winType);
	std::string MakePlayerList();

	virtual void RegisterGame(cNetworkPlayer* player) override;
	virtual void UnRegisterGame(cNetworkPlayer* player) override;
	virtual void AddMessage(cNetworkPlayer* player, std::string message) override;
	virtual void InputAction(cNetworkPlayer* player, ePlayerAction actionState, std::string targetName) override;
};

