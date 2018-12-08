// ConsoleGame.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "cMain.h"

//게임종료 전역변수
bool g_isQuit = false;

int main()
{
	//메인 클래스 동적 할당
	cMain* pMain = new cMain;
	//메인 클래스 Init 함수 호출
	pMain->Init();

	clock_t culTime, oldTime = clock();

	ScreenInit();

	//게임 루프
	while (!g_isQuit)//게임종료 전역변수가 true가 되면 게임루프종료 -> 프로그램 종료
	{
		//콘솔창 문자 지우기
		//system("cls");
		//버퍼 클리어
		ScreenClear();
		//메인 클래스 Update 함수 호출
		pMain->Update();
		//메인 클래스 Render 함수 호출
		pMain->Render();

		//버퍼 체인지
		ScreenFlipping();
		//게임 루프 딜레이 주기(단위 ms -> 1000ms = 1s) 현재 30프레임 게임 루프
		//Sleep(100);
		//게임 루프 딜레이 주기(단위 ms -> 1000ms = 1s) 현재 30프레임 게임 루프
		while (true)
		{
			culTime = clock();
			if (culTime - oldTime > 33)
			{
				oldTime = culTime;
				break;
			}
		}
	}

	//렌더버퍼 릴리즈
	ScreenRelease();

	//메인클래스 메모리 해제
	delete pMain;


	//프로그램 정로
    return 0;
}

