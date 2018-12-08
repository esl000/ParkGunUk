#include "stdafx.h"
#include "cMafiaGame.h"
#include "cNetworkPlayer.h"


cMafiaGame::cMafiaGame()
	: m_GameState(eGameState::E_LOBBY)
	, m_checkedPlayer(false)
{
}


cMafiaGame::~cMafiaGame()
{
	for (auto player : m_vecPlayer)
	{
		delete player;
	}
}

cMafiaGame* cMafiaGame::GetInstance()
{
	static cMafiaGame instance;
	return &instance;
}

void cMafiaGame::GameStart()
{
	while (true)
	{
		if (m_vecPlayer.size() == 3 && m_vecPlayerByCharacter.size() == 3)
		{
			break;
		}
	}

	SendSystemMessage(std::string("게임이 시작되었습니다."));
	SendSystemMessage(MakePlayerList());
	JobSetting();

	while (true)
	{
		g_MainMutex.lock();
		m_GameState = eGameState::E_DAY;
		g_MainMutex.unlock();
		SendSystemMessage(std::string("낮이 되었습니다. 플레이어들은 1분간 자유롭게 이야기할 수 있습니다."));
		Sleep(60000);


		m_votePlayer.clear();
		m_voteTargetPlayer.clear();

		g_MainMutex.lock();
		m_GameState = eGameState::E_VOTETIME;
		g_MainMutex.unlock();

		SendSystemMessage(std::string("투표시간입니다. 플레이어들은 30초간 투표를 진행해주세요."));
		Sleep(30000);


		//g_MainMutex.lock();
		//투표처리

		std::vector<cNetworkPlayer*> votedPlayer;
		int maxVote = 0;
		for (auto votePair : m_voteTargetPlayer)
		{
			if (votePair.second == maxVote)
			{
				votedPlayer.push_back(votePair.first);
			}
			else if(votePair.second > maxVote)
			{
				votedPlayer.clear();
				votedPlayer.push_back(votePair.first);
				maxVote = votePair.second;
			}
		}
		if (votedPlayer.size() == 1)
		{

			SendSystemMessage(std::string("투표가 끝났습니다. 투표 결과 [" + votedPlayer[0]->PlayerName() + "] 님이 죽으셨습니다."));
			SendSystemMessage(votedPlayer[0], "당신은 죽었습니다.");	

			SetPlayerDie(votedPlayer[0]);
			if (isGameEnd(m_CurrentGameWin))
				break;		
		}
		else
			SendSystemMessage(std::string("투표가 끝났습니다. 투표결과는 무효처리되었습니다."));

		m_GameState = eGameState::E_NIGHT;

		//g_MainMutex.unlock();





		g_MainMutex.lock();
		m_bCheckedPlayer = false;
		m_bRenderCheckedPlayerJob = true;
		m_checkedPlayer = nullptr;
		g_MainMutex.unlock();

		SendSystemMessage(std::string("밤이 되었습니다. 플레이어들은 30초 동안 각자 스킬을 사용하실 수 있습니다."));
		Sleep(30000);

		if (m_mafiaKilledPlayer != nullptr)
		{
			SendSystemMessage(std::string("마피아 들에 의해 [" + m_mafiaKilledPlayer->PlayerName() + "] 님이 죽었습니다."));
			SetPlayerDie(m_mafiaKilledPlayer);
		}
		m_mafiaKilledPlayer = nullptr;
		if (isGameEnd(m_CurrentGameWin))
			break;




	}

	if (m_CurrentGameWin == eGameWinType::E_MAFIAWIN)
	{
		SendSystemMessage("마피아 승리!");
	}
	else if (m_CurrentGameWin == eGameWinType::E_PERSONWIN)
	{
		SendSystemMessage("시민 승리!");
	}

}

void cMafiaGame::ConnectSocket(SOCKET& clientSocket)
{
	cNetworkPlayer* player = new cNetworkPlayer;
	RegisterGame(player);

	g_MainMutex.lock();
	m_vecPlayer.insert(player);
	g_MainMutex.unlock();

	std::cout << "player connected!" << std::endl;
	player->Start(clientSocket);
}

void cMafiaGame::SendSystemMessage(cNetworkPlayer * player, std::string message)
{
	message.append("\n");
	message = "[System] : " + message;
	player->SendMessageToClient(message);
}

void cMafiaGame::SendSystemMessage(std::string message)
{
	message.append("\n");
	message = "[System] : " + message;

	g_MainMutex.lock();
	for (auto players : m_vecPlayer)
	{
		players->SendMessageToClient(message);
	}
	g_MainMutex.unlock();
}

void cMafiaGame::SetPlayerDie(cNetworkPlayer* player)
{
	g_MainMutex.lock();
	player->PlayerState(ePlayerState::E_DEAD);
	g_MainMutex.unlock();

	m_CurrentGameWin = CheckGameWin();
}

void cMafiaGame::JobSetting()
{
	srand((unsigned int)time(NULL));
	std::string jobInfo[ePlayerJob::E_JOBLENGTH] =
	{
		"당신은 시민입니다.\n모든 마피아를 죽이면 승리합니다.\n특수능력은 없습니다.",
		"당신은 마피아입니다.\n마피아가 다른 사람의 수보다 같거나 많으면 이깁니다.\n저녁에 -kill <플레이어이름> 이라는 명령어로 사람을 죽일 수 있습니다.",
		"당신은 경찰입니다.\n모든 마피아를 죽이면 승리합니다.\n저녁에 -check <플레이어이름> 이라는 명령어로 다른사람의 직업을 확인 할 수 있습니다."
	};

	ePlayerJob jopShuffle[ePlayerJob::E_JOBLENGTH] = { ePlayerJob::E_PERSON, ePlayerJob::E_MAFIA, ePlayerJob::E_POLICE};

	int i = 0;
	for (i = 0; i < 10; ++i)
	{
		int randIndex1 = rand() % ePlayerJob::E_JOBLENGTH, randIndex2 = rand() % ePlayerJob::E_JOBLENGTH;
		ePlayerJob temp = jopShuffle[randIndex1];
		jopShuffle[randIndex1] = jopShuffle[randIndex2];
		jopShuffle[randIndex2] = temp;
	}

	i = 0;
	for (auto player : m_vecPlayer)
	{
		player->PlayerJob(jopShuffle[i]);
		SendSystemMessage(player, jobInfo[jopShuffle[i++]]);
	}
}

eGameWinType cMafiaGame::CheckGameWin()
{
	int mafiaAlive = 0;
	int personAlive = 0;
	for (auto player : m_vecPlayer)
	{
		if (player->PlayerJob() == ePlayerJob::E_MAFIA && player->PlayerState() == ePlayerState::E_ALIVE)
			++mafiaAlive;
		if (player->PlayerJob() != ePlayerJob::E_MAFIA && player->PlayerState() == ePlayerState::E_ALIVE)
			++personAlive;
	}

	if (mafiaAlive >= personAlive)
		return eGameWinType::E_MAFIAWIN;

	if (mafiaAlive == 0)
		return eGameWinType::E_PERSONWIN;

	return eGameWinType::E_NOTEND;
}

bool cMafiaGame::isGameEnd(eGameWinType winType)
{
	if (winType != 0)
		return true;

	return false;
}

std::string cMafiaGame::MakePlayerList()
{
	std::string outMessage = "플레이어 명단--------------------\n";
	for (auto player : m_vecPlayer)
	{
		g_MainMutex.lock();
		outMessage.append(player->PlayerName() + "\n");
		g_MainMutex.unlock();
	}
	outMessage.append("플레이어 명단--------------------\n");

	return outMessage;
}

void cMafiaGame::RegisterGame(cNetworkPlayer* player)
{
	player->Delegate(this);
}

void cMafiaGame::UnRegisterGame(cNetworkPlayer* player)
{
	g_MainMutex.lock();
	m_vecPlayer.erase(player);
	m_vecPlayerByCharacter.erase(player->PlayerName());
	g_MainMutex.unlock();
}

void cMafiaGame::AddMessage(cNetworkPlayer* player, std::string message)
{
	if (m_GameState == eGameState::E_NIGHT)
	{
		return;
	}

	message.append("\n");
	if(player != nullptr)
		message = "[" + player->PlayerName() + "] : " + message;

	std::cout << message.data() << std::endl;


	g_MainMutex.lock();
	for (auto players : m_vecPlayer)
	{
		players->SendMessageToClient(message);
	}
	g_MainMutex.unlock();
}

void cMafiaGame::InputAction(cNetworkPlayer * player, ePlayerAction actionState, std::string targetName)
{


	std::string actionInfoMessage;

	std::cout << "command - " << (int)actionState << " put" << std::endl;

	g_MainMutex.lock();
	switch (actionState)
	{
	case ePlayerAction::E_VOTE:
		if (m_GameState != eGameState::E_VOTETIME)
		{
			actionInfoMessage = "투표시간이 아닙니다.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "타겟이름이 명단에 없습니다.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "타겟은 이미 죽었습니다.";
			break;
		}
		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;


		actionInfoMessage = targetName + " 에게 투표했습니다.";
		if(m_voteTargetPlayer.find(m_vecPlayerByCharacter[targetName]) == m_voteTargetPlayer.end())
			m_voteTargetPlayer.insert(std::pair<cNetworkPlayer*, int>(m_vecPlayerByCharacter[targetName], 1));
		else
			m_voteTargetPlayer[m_vecPlayerByCharacter[targetName]] += 1;

		break;
	case ePlayerAction::E_KILL:
		if (m_GameState != eGameState::E_NIGHT)
		{
			actionInfoMessage = "밤이 아닙니다.";
			break;
		}
		if (player->PlayerJob() != ePlayerJob::E_MAFIA)
		{
			actionInfoMessage = "명령어를 사용할 권한이 없습니다.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "타겟이름이 명단에 없습니다.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "타겟은 이미 죽었습니다.";
			break;
		}

		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;

		if(m_mafiaKilledPlayer != nullptr)
			actionInfoMessage = targetName + " 으로 타겟을 변경했습니다.";
		else
			actionInfoMessage = targetName + " 을 타겟으로 지정했습니다.";

		m_mafiaKilledPlayer = m_vecPlayerByCharacter[targetName];
		break;
	case ePlayerAction::E_CHECKJOB:
		if (m_GameState != eGameState::E_NIGHT)
		{
			actionInfoMessage = "밤이 아닙니다.";
			break;
		}
		if (player->PlayerJob() != ePlayerJob::E_POLICE)
		{
			actionInfoMessage = "명령어를 사용할 권한이 없습니다.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "타겟이름이 명단에 없습니다.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "타겟은 이미 죽었습니다.";
			break;
		}
		if (m_bCheckedPlayer)
		{
			actionInfoMessage = "이미 능력을 사용하였습니다.";
			break;
		}

		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;

		m_bCheckedPlayer = true;
		m_bRenderCheckedPlayerJob = false;
		m_checkedPlayer = m_vecPlayerByCharacter[targetName];

		if (m_checkedPlayer->PlayerJob() == ePlayerJob::E_MAFIA)
			actionInfoMessage = m_checkedPlayer->PlayerName() + " 님은 마피아입니다.";
		else
			actionInfoMessage = m_checkedPlayer->PlayerName() + " 님은 마피아가 아닙니다.";

		break;
	case ePlayerAction::E_PLAYERSETTING:
		player->PlayerName(targetName);
		m_vecPlayerByCharacter.insert(std::pair<std::string, cNetworkPlayer*>(targetName, player));
		std::cout << "player : " << targetName.data() << " register!" << std::endl;

		break;
	}
	g_MainMutex.unlock();

	SendSystemMessage(player, actionInfoMessage);
}