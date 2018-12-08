// ConsoleGame.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "cMain.h"

//�������� ��������
bool g_isQuit = false;

int main()
{
	//���� Ŭ���� ���� �Ҵ�
	cMain* pMain = new cMain;
	//���� Ŭ���� Init �Լ� ȣ��
	pMain->Init();

	clock_t culTime, oldTime = clock();

	ScreenInit();

	//���� ����
	while (!g_isQuit)//�������� ���������� true�� �Ǹ� ���ӷ������� -> ���α׷� ����
	{
		//�ܼ�â ���� �����
		//system("cls");
		//���� Ŭ����
		ScreenClear();
		//���� Ŭ���� Update �Լ� ȣ��
		pMain->Update();
		//���� Ŭ���� Render �Լ� ȣ��
		pMain->Render();

		//���� ü����
		ScreenFlipping();
		//���� ���� ������ �ֱ�(���� ms -> 1000ms = 1s) ���� 30������ ���� ����
		//Sleep(100);
		//���� ���� ������ �ֱ�(���� ms -> 1000ms = 1s) ���� 30������ ���� ����
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

	//�������� ������
	ScreenRelease();

	//����Ŭ���� �޸� ����
	delete pMain;


	//���α׷� ����
    return 0;
}

