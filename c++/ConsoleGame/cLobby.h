#pragma once
#include "iScene.h"

//�� �߻� Ŭ������ ��ӹ��� Ŭ����
struct cLobby : public iScene
{
	//�κ� �޼���
	char* m_szLobby;
public:
	cLobby();
	~cLobby();

	virtual void Init();
	virtual void Update();
	virtual void Render();
};

