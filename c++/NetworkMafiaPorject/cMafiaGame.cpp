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

	SendSystemMessage(std::string("������ ���۵Ǿ����ϴ�."));
	SendSystemMessage(MakePlayerList());
	JobSetting();

	while (true)
	{
		g_MainMutex.lock();
		m_GameState = eGameState::E_DAY;
		g_MainMutex.unlock();
		SendSystemMessage(std::string("���� �Ǿ����ϴ�. �÷��̾���� 1�а� �����Ӱ� �̾߱��� �� �ֽ��ϴ�."));
		Sleep(60000);


		m_votePlayer.clear();
		m_voteTargetPlayer.clear();

		g_MainMutex.lock();
		m_GameState = eGameState::E_VOTETIME;
		g_MainMutex.unlock();

		SendSystemMessage(std::string("��ǥ�ð��Դϴ�. �÷��̾���� 30�ʰ� ��ǥ�� �������ּ���."));
		Sleep(30000);


		//g_MainMutex.lock();
		//��ǥó��

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

			SendSystemMessage(std::string("��ǥ�� �������ϴ�. ��ǥ ��� [" + votedPlayer[0]->PlayerName() + "] ���� �����̽��ϴ�."));
			SendSystemMessage(votedPlayer[0], "����� �׾����ϴ�.");	

			SetPlayerDie(votedPlayer[0]);
			if (isGameEnd(m_CurrentGameWin))
				break;		
		}
		else
			SendSystemMessage(std::string("��ǥ�� �������ϴ�. ��ǥ����� ��ȿó���Ǿ����ϴ�."));

		m_GameState = eGameState::E_NIGHT;

		//g_MainMutex.unlock();





		g_MainMutex.lock();
		m_bCheckedPlayer = false;
		m_bRenderCheckedPlayerJob = true;
		m_checkedPlayer = nullptr;
		g_MainMutex.unlock();

		SendSystemMessage(std::string("���� �Ǿ����ϴ�. �÷��̾���� 30�� ���� ���� ��ų�� ����Ͻ� �� �ֽ��ϴ�."));
		Sleep(30000);

		if (m_mafiaKilledPlayer != nullptr)
		{
			SendSystemMessage(std::string("���Ǿ� �鿡 ���� [" + m_mafiaKilledPlayer->PlayerName() + "] ���� �׾����ϴ�."));
			SetPlayerDie(m_mafiaKilledPlayer);
		}
		m_mafiaKilledPlayer = nullptr;
		if (isGameEnd(m_CurrentGameWin))
			break;




	}

	if (m_CurrentGameWin == eGameWinType::E_MAFIAWIN)
	{
		SendSystemMessage("���Ǿ� �¸�!");
	}
	else if (m_CurrentGameWin == eGameWinType::E_PERSONWIN)
	{
		SendSystemMessage("�ù� �¸�!");
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
		"����� �ù��Դϴ�.\n��� ���ǾƸ� ���̸� �¸��մϴ�.\nƯ���ɷ��� �����ϴ�.",
		"����� ���Ǿ��Դϴ�.\n���Ǿư� �ٸ� ����� ������ ���ų� ������ �̱�ϴ�.\n���ῡ -kill <�÷��̾��̸�> �̶�� ��ɾ�� ����� ���� �� �ֽ��ϴ�.",
		"����� �����Դϴ�.\n��� ���ǾƸ� ���̸� �¸��մϴ�.\n���ῡ -check <�÷��̾��̸�> �̶�� ��ɾ�� �ٸ������ ������ Ȯ�� �� �� �ֽ��ϴ�."
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
	std::string outMessage = "�÷��̾� ���--------------------\n";
	for (auto player : m_vecPlayer)
	{
		g_MainMutex.lock();
		outMessage.append(player->PlayerName() + "\n");
		g_MainMutex.unlock();
	}
	outMessage.append("�÷��̾� ���--------------------\n");

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
			actionInfoMessage = "��ǥ�ð��� �ƴմϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "Ÿ���̸��� ��ܿ� �����ϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "Ÿ���� �̹� �׾����ϴ�.";
			break;
		}
		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;


		actionInfoMessage = targetName + " ���� ��ǥ�߽��ϴ�.";
		if(m_voteTargetPlayer.find(m_vecPlayerByCharacter[targetName]) == m_voteTargetPlayer.end())
			m_voteTargetPlayer.insert(std::pair<cNetworkPlayer*, int>(m_vecPlayerByCharacter[targetName], 1));
		else
			m_voteTargetPlayer[m_vecPlayerByCharacter[targetName]] += 1;

		break;
	case ePlayerAction::E_KILL:
		if (m_GameState != eGameState::E_NIGHT)
		{
			actionInfoMessage = "���� �ƴմϴ�.";
			break;
		}
		if (player->PlayerJob() != ePlayerJob::E_MAFIA)
		{
			actionInfoMessage = "��ɾ ����� ������ �����ϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "Ÿ���̸��� ��ܿ� �����ϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "Ÿ���� �̹� �׾����ϴ�.";
			break;
		}

		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;

		if(m_mafiaKilledPlayer != nullptr)
			actionInfoMessage = targetName + " ���� Ÿ���� �����߽��ϴ�.";
		else
			actionInfoMessage = targetName + " �� Ÿ������ �����߽��ϴ�.";

		m_mafiaKilledPlayer = m_vecPlayerByCharacter[targetName];
		break;
	case ePlayerAction::E_CHECKJOB:
		if (m_GameState != eGameState::E_NIGHT)
		{
			actionInfoMessage = "���� �ƴմϴ�.";
			break;
		}
		if (player->PlayerJob() != ePlayerJob::E_POLICE)
		{
			actionInfoMessage = "��ɾ ����� ������ �����ϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter.find(targetName) == m_vecPlayerByCharacter.end())
		{
			actionInfoMessage = "Ÿ���̸��� ��ܿ� �����ϴ�.";
			break;
		}
		if (m_vecPlayerByCharacter[targetName]->PlayerState() == ePlayerState::E_DEAD)
		{
			actionInfoMessage = "Ÿ���� �̹� �׾����ϴ�.";
			break;
		}
		if (m_bCheckedPlayer)
		{
			actionInfoMessage = "�̹� �ɷ��� ����Ͽ����ϴ�.";
			break;
		}

		std::cout << "command - " << (int)actionState << " start rogic" << std::endl;

		m_bCheckedPlayer = true;
		m_bRenderCheckedPlayerJob = false;
		m_checkedPlayer = m_vecPlayerByCharacter[targetName];

		if (m_checkedPlayer->PlayerJob() == ePlayerJob::E_MAFIA)
			actionInfoMessage = m_checkedPlayer->PlayerName() + " ���� ���Ǿ��Դϴ�.";
		else
			actionInfoMessage = m_checkedPlayer->PlayerName() + " ���� ���Ǿư� �ƴմϴ�.";

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