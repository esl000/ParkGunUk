#include "stdafx.h"
#include "cStage.h"


cStage::cStage()
	:m_vecMap(NULL)
	,m_pPlayer(NULL)
{
	m_dwLevelIndex = 0;
}


cStage::~cStage()
{
	//���߹迭�� �ִ� ������Ʈ ����ü �޸� ����
	for (size_t i = 0; i < m_vecMap.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMap[i].size(); ++j)
		{
			delete m_vecMap[i][j];
		}
	}

	//��ֹ� �迭�� �ִ� ��ֹ� �޸� ����
	for (size_t i = 0; i < m_vecEnemy.size(); ++i)
	{
		delete m_vecEnemy[i];
	}

	//�÷��̾� �޸� ����
	delete m_pPlayer;
}

//���� Ŭ������ �ʱ�ȭ �Լ�
//���� �Ľ��� ���� ���� �ҷ��´�.

//10 <- �� �� ��
//20 <- �� �� ��
//5 <- ��ֹ� ����
//11111111111111111111 <- 0��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11110000000000002221 <- 1��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11110000000000002221 <- 2��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11110000000000002221 <- 3��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11110000000000001111 <- 4��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11110000000000001111 <- 5��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//10000000000000001111 <- 6��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//10000000000000001111 <- 7��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//10000000000000001111 <- 8��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//11111111111111111111 <- 9��° ���� �� ������Ʈ(0 == ���, 1 == ��, 2 == ���� Ŭ���� ����) 
//2,7 <- �÷��̾� ������ġ (2��° ���� 7��° ��)
//2 5,1 5,8 <- 0��° ��ֹ��� ���� ((��ֹ� ������ ����) (0��°������ ��ġ(5,1)) ...) //�̷��� ��ֹ��� 0��° ���������� ����Ͽ� ������ ������� �̵��ϰ� �������� �ٽ� 0��° �������� ���� ��ȯ���� �������� �߻��ϰԵȴ�.
//2 7,1 7,8
//2 10,1 10,8
//2 13,1 13,8
//2 15,1 15,8

void cStage::Init()
{
	//���� ��ο� ���ڿ� ����
	char szPath[128];
	//���� ��� ���ڿ� ����(�����ε����� 0 �̸� 0.txt, �����ε����� 1 �̸� 1.txt)
	sprintf_s(szPath, "%d.txt", (int)m_dwLevelIndex);

	//���� ������ ����
	FILE* fp;
	//���� ����
	fopen_s(&fp, szPath, "r");

	//���� ������ �ȿ������ (�غ��� ��� ���������� �ٱ��� �ε����� �ʰ��� ���)
	//��������
	if (!fp)
	{
		system("cls");
		cout << "���� ����" << endl;
		g_isQuit = true;
		return;
	}

	//���� ��, ��, ��ֹ� ����
	int nLine, nRaw, nEnemy = 0;
	char szBuf[1024];

	//������ ������ �о�´�.
	fgets(szBuf, 1024, fp);
	//�о�� ���ڿ��� ���ڷ� �ٲ۰� ���� ��
	nLine = atoi(szBuf);

	//���� ���� vector�� ũ�⸦ ���Ѵ�.
	m_vecMap.resize(nLine);

	//������ �̾ ������ �о�´�.
	fgets(szBuf, 1024, fp);
	//�о�� ���ڿ��� ���ڷ� �ٲ۰� ���� ��
	nRaw = atoi(szBuf);

	//������ �̾ ������ �о�´�.
	fgets(szBuf, 1024, fp);
	//�о�� ���ڿ��� ���ڷ� �ٲ۰� ��ֹ��� ��
	nEnemy = atoi(szBuf);

	//��ֹ� vector�� ũ�����ϱ� 
	m_vecEnemy.resize(nEnemy);

	//���ڿ� �ڸ���� ĳ���� ������ ������
	char* szToken = NULL;
	char* szContext = NULL;
	char* szPosContext = NULL;
	char* szPosToken = NULL;

	//���� ��ȯ�� ���ڿ�
	char szNum[128];

	for (int i = 0; i < nLine; ++i)
	{
		//������ �̾ ������ �о�´�.
		fgets(szBuf, 1024, fp);

		//���� i��°���� ���� ũ�⸦ ���Ѵ�.
		m_vecMap[i].resize(nRaw);

		//�� �ึ�� ������Ʈ �ֱ� ����
		for (int j = 0; j < nRaw; ++j)
		{
			szNum[0] = szBuf[j];
			szNum[1] = 0;
			m_vecMap[i][j] = new ST_OBJECT((eObjectType)(int)atoi(szNum), Vecter2(j, i));
		}
	}
	//��ֹ� ������ ������
	int nDestSize;

	fgets(szBuf, 1024, fp);

	//�÷��̾� ������ġ ����
	szPosToken = strtok_s(szBuf, ",", &szPosContext);
	Vecter2 vPos(atoi(szPosToken), atoi(szPosContext));

	m_pPlayer = new ST_PLAYER(E_PLAYER, vPos);

	//��ֹ� �ʱ�ȭ
	for (int i = 0; i < nEnemy; ++i)
	{
		fgets(szBuf, 1024, fp);
		//" "�� ��ū���� �ؼ� ���ڿ��� �ڸ���.
		szToken = strtok_s(szBuf, " ", &szContext);
		//ù��°�� ������ ������
		nDestSize = atoi(szToken);

		m_vecEnemy[i] = new ST_ENEMY(E_ENEMY, 0);
		//��ֹ��� ������ �迭 ũ�� ���ϱ�
		m_vecEnemy[i]->m_vecDest.resize(nDestSize);

		//������ ���� �θ���
		for (int j = 0; j < nDestSize; ++j)
		{
			//" "�� ��ū���� �ؼ� ���ڿ��� �ڸ���.(ex 0,1)
			szToken = strtok_s(szContext, " ", &szContext);
			//","�� ��ū���� �ؼ� ���ڿ��� �ڸ���.(ex 0 �ϰ� 1)
			szPosToken = strtok_s(szToken, ",", &szPosContext);

			//������ ��ġ�� �����Ѵ�.
			vPos.m_nX = atoi(szPosToken);
			vPos.m_nY = atoi(szPosContext);

			//������ �迭�� �־��ش�.
			m_vecEnemy[i]->m_vecDest[j] = vPos;
		}
		// ��ֹ��� ó����ġ�� ������ �迭�� 0��° ��Ҵ�.
		m_vecEnemy[i]->m_vPos = m_vecEnemy[i]->m_vecDest[0];
	}
}

void cStage::Update()
{
	if (m_pPlayer->Update())
	{
		//�� Ű�� ������
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			//�켱�̵�
			--m_pPlayer->m_vPos.m_nX;
			//���̶� ��ġ�� �̵����
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				++m_pPlayer->m_vPos.m_nX;
			}
		}
	//�� Ű�� ������
		else if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			++m_pPlayer->m_vPos.m_nX;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				--m_pPlayer->m_vPos.m_nX;
			}
		}
	//�� Ű�� ������
		else if (GetKeyState(VK_UP) & 0x8000)
		{
			--m_pPlayer->m_vPos.m_nY;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				++m_pPlayer->m_vPos.m_nY;
			}
		}
	//�� Ű�� ������
		else if (GetKeyState(VK_DOWN) & 0x8000)
		{
			++m_pPlayer->m_vPos.m_nY;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				--m_pPlayer->m_vPos.m_nY;
			}
		}
	}

	//��ֹ���� ��ġ�� ��ġ���� Ȯ��
	for (size_t i = 0; i < m_vecEnemy.size(); ++i)
	{
		//���࿡ ��ġ��
		if (m_vecEnemy[i]->m_vPos == m_pPlayer->m_vPos)
		{
			//��������
			g_isQuit = true;
		}
		m_vecEnemy[i]->Update();
		//���࿡ ��ġ��
		if (m_vecEnemy[i]->m_vPos == m_pPlayer->m_vPos)
		{
			//��������
			g_isQuit = true;
		}
	}

	//����Ŭ���� ��ġ�� �ִ��� Ȯ��
	if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_FINISH)
	{
		//�������� �Ѿ��(���� �ε��� ++)
		m_pDelegate->OnLevelFinished();
	}
}

//������
void cStage::Render()
{
	//���߹迭�� �ִ¿�����Ʈ �׸���
	for (size_t i = 0; i < m_vecMap.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMap[i].size(); ++j)
		{
			//���� �ش���ġ�� �÷��̾ ������
			if (m_pPlayer->m_vPos == Vecter2(j, i))
			{
				//�÷��̾� ����
				m_pPlayer->Render();
				continue;
			}

			bool m_isContinue = false;

			//��ֹ��鵵 ��ġ��
			for (size_t k = 0; k < m_vecEnemy.size(); ++k)
			{
				//�ش���ġ�� ��ֹ����� ������
				if (m_vecEnemy[k]->m_vPos == Vecter2(j, i))
				{
					//��ֹ�����
					m_vecEnemy[k]->Render();
					m_isContinue = true;
					break;
				}
			}

			if(m_isContinue)
				continue;

			//�÷��̾ ���� ��ֹ��� ������ �� ������Ʈ ����
			m_vecMap[i][j]->Render();
			
		}
		//cout << endl;
	}
	char strStageIndex[20];
	strStageIndex[0] = (char)((int)m_dwLevelIndex + '0');
	strStageIndex[1] = 0;

	string mapInfo = "�������� : ";
	mapInfo.append(strStageIndex);

	ScreenPrint(0, m_vecMap.size(), &mapInfo[0]);
	//cout << "�������� : " << m_dwLevelIndex << endl;
}
