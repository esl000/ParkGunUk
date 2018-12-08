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
	//이중배열에 있는 오브젝트 구조체 메모리 해제
	for (size_t i = 0; i < m_vecMap.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMap[i].size(); ++j)
		{
			delete m_vecMap[i][j];
		}
	}

	//장애물 배열에 있는 장애물 메모리 해제
	for (size_t i = 0; i < m_vecEnemy.size(); ++i)
	{
		delete m_vecEnemy[i];
	}

	//플레이어 메모리 해제
	delete m_pPlayer;
}

//레벨 클래스의 초기화 함수
//파일 파싱을 통해 맵을 불러온다.

//10 <- 맵 줄 수
//20 <- 맵 행 수
//5 <- 장애물 갯수
//11111111111111111111 <- 0번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11110000000000002221 <- 1번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11110000000000002221 <- 2번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11110000000000002221 <- 3번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11110000000000001111 <- 4번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11110000000000001111 <- 5번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//10000000000000001111 <- 6번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//10000000000000001111 <- 7번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//10000000000000001111 <- 8번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//11111111111111111111 <- 9번째 줄의 맵 오브젝트(0 == 빈곳, 1 == 벽, 2 == 게임 클리어 발판) 
//2,7 <- 플레이어 시작위치 (2번째 줄의 7번째 열)
//2 5,1 5,8 <- 0번째 장애물의 정보 ((장애물 목적지 갯수) (0번째목적지 위치(5,1)) ...) //이러면 장애물은 0번째 목적지에서 출발하여 목적지 순서대로 이동하고 마지막은 다시 0번째 목적지로 오는 순환적인 움직임이 발생하게된다.
//2 7,1 7,8
//2 10,1 10,8
//2 13,1 13,8
//2 15,1 15,8

void cStage::Init()
{
	//파일 경로용 문자열 생성
	char szPath[128];
	//파일 경로 문자열 제작(레벨인덱스가 0 이면 0.txt, 레벨인덱스가 1 이면 1.txt)
	sprintf_s(szPath, "%d.txt", (int)m_dwLevelIndex);

	//파일 포인터 생성
	FILE* fp;
	//파일 오픈
	fopen_s(&fp, szPath, "r");

	//만약 파일이 안열릴경우 (준비한 모든 스테이지를 다깨고 인덱스가 초과한 경우)
	//게임종료
	if (!fp)
	{
		system("cls");
		cout << "게임 엔딩" << endl;
		g_isQuit = true;
		return;
	}

	//맵의 줄, 행, 장애물 갯수
	int nLine, nRaw, nEnemy = 0;
	char szBuf[1024];

	//파일을 한줄을 읽어온다.
	fgets(szBuf, 1024, fp);
	//읽어온 문자열을 숫자로 바꾼게 줄의 수
	nLine = atoi(szBuf);

	//줄의 수로 vector의 크기를 정한다.
	m_vecMap.resize(nLine);

	//파일을 이어서 한줄을 읽어온다.
	fgets(szBuf, 1024, fp);
	//읽어온 문자열을 숫자로 바꾼게 행의 수
	nRaw = atoi(szBuf);

	//파일을 이어서 한줄을 읽어온다.
	fgets(szBuf, 1024, fp);
	//읽어온 문자열을 숫자로 바꾼게 장애물의 수
	nEnemy = atoi(szBuf);

	//장애물 vector의 크기정하기 
	m_vecEnemy.resize(nEnemy);

	//문자열 자르기용 캐릭터 포인터 변수들
	char* szToken = NULL;
	char* szContext = NULL;
	char* szPosContext = NULL;
	char* szPosToken = NULL;

	//숫자 변환용 문자열
	char szNum[128];

	for (int i = 0; i < nLine; ++i)
	{
		//파일을 이어서 한줄을 읽어온다.
		fgets(szBuf, 1024, fp);

		//맵의 i번째줄의 행의 크기를 정한다.
		m_vecMap[i].resize(nRaw);

		//각 행마다 오브젝트 넣기 로직
		for (int j = 0; j < nRaw; ++j)
		{
			szNum[0] = szBuf[j];
			szNum[1] = 0;
			m_vecMap[i][j] = new ST_OBJECT((eObjectType)(int)atoi(szNum), Vecter2(j, i));
		}
	}
	//쟁애물 목적지 사이즈
	int nDestSize;

	fgets(szBuf, 1024, fp);

	//플레이어 시작위치 설정
	szPosToken = strtok_s(szBuf, ",", &szPosContext);
	Vecter2 vPos(atoi(szPosToken), atoi(szPosContext));

	m_pPlayer = new ST_PLAYER(E_PLAYER, vPos);

	//장애물 초기화
	for (int i = 0; i < nEnemy; ++i)
	{
		fgets(szBuf, 1024, fp);
		//" "를 토큰으로 해서 문자열을 자른다.
		szToken = strtok_s(szBuf, " ", &szContext);
		//첫번째는 목적지 사이즈
		nDestSize = atoi(szToken);

		m_vecEnemy[i] = new ST_ENEMY(E_ENEMY, 0);
		//장애물의 목적지 배열 크지 정하기
		m_vecEnemy[i]->m_vecDest.resize(nDestSize);

		//목저지 정보 부르기
		for (int j = 0; j < nDestSize; ++j)
		{
			//" "를 토큰으로 해서 문자열을 자른다.(ex 0,1)
			szToken = strtok_s(szContext, " ", &szContext);
			//","를 토큰으로 해서 문자열을 자른다.(ex 0 하고 1)
			szPosToken = strtok_s(szToken, ",", &szPosContext);

			//목적지 위치를 저장한다.
			vPos.m_nX = atoi(szPosToken);
			vPos.m_nY = atoi(szPosContext);

			//목적지 배열에 넣어준다.
			m_vecEnemy[i]->m_vecDest[j] = vPos;
		}
		// 장애물의 처음위치는 목저지 배열의 0번째 요소다.
		m_vecEnemy[i]->m_vPos = m_vecEnemy[i]->m_vecDest[0];
	}
}

void cStage::Update()
{
	if (m_pPlayer->Update())
	{
		//← 키를 누를때
		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			//우선이동
			--m_pPlayer->m_vPos.m_nX;
			//벽이랑 겹치면 이동취소
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				++m_pPlayer->m_vPos.m_nX;
			}
		}
	//→ 키를 누를때
		else if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			++m_pPlayer->m_vPos.m_nX;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				--m_pPlayer->m_vPos.m_nX;
			}
		}
	//↑ 키를 누를때
		else if (GetKeyState(VK_UP) & 0x8000)
		{
			--m_pPlayer->m_vPos.m_nY;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				++m_pPlayer->m_vPos.m_nY;
			}
		}
	//↓ 키를 누를때
		else if (GetKeyState(VK_DOWN) & 0x8000)
		{
			++m_pPlayer->m_vPos.m_nY;
			if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_WALL)
			{
				--m_pPlayer->m_vPos.m_nY;
			}
		}
	}

	//장애물들과 위치가 겹치는지 확인
	for (size_t i = 0; i < m_vecEnemy.size(); ++i)
	{
		//만약에 겹치면
		if (m_vecEnemy[i]->m_vPos == m_pPlayer->m_vPos)
		{
			//게임종료
			g_isQuit = true;
		}
		m_vecEnemy[i]->Update();
		//만약에 겹치면
		if (m_vecEnemy[i]->m_vPos == m_pPlayer->m_vPos)
		{
			//게임종료
			g_isQuit = true;
		}
	}

	//게임클리어 위치에 있는지 확인
	if (m_vecMap[m_pPlayer->m_vPos.m_nY][m_pPlayer->m_vPos.m_nX]->m_eType == E_FINISH)
	{
		//스테이지 넘어가기(레벨 인덱스 ++)
		m_pDelegate->OnLevelFinished();
	}
}

//렌더링
void cStage::Render()
{
	//이중배열에 있는오브젝트 그리기
	for (size_t i = 0; i < m_vecMap.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMap[i].size(); ++j)
		{
			//만약 해당위치에 플레이어가 있으면
			if (m_pPlayer->m_vPos == Vecter2(j, i))
			{
				//플레이어 렌더
				m_pPlayer->Render();
				continue;
			}

			bool m_isContinue = false;

			//장애물들도 위치비교
			for (size_t k = 0; k < m_vecEnemy.size(); ++k)
			{
				//해당위치에 장애물들이 있으면
				if (m_vecEnemy[k]->m_vPos == Vecter2(j, i))
				{
					//장애물렌더
					m_vecEnemy[k]->Render();
					m_isContinue = true;
					break;
				}
			}

			if(m_isContinue)
				continue;

			//플레이어도 없고 장애물도 없으면 맵 오브젝트 렌더
			m_vecMap[i][j]->Render();
			
		}
		//cout << endl;
	}
	char strStageIndex[20];
	strStageIndex[0] = (char)((int)m_dwLevelIndex + '0');
	strStageIndex[1] = 0;

	string mapInfo = "스테이지 : ";
	mapInfo.append(strStageIndex);

	ScreenPrint(0, m_vecMap.size(), &mapInfo[0]);
	//cout << "스테이지 : " << m_dwLevelIndex << endl;
}
