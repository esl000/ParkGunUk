#pragma once
#include "iScene.h"

//씬 추상 클래스를 상속받은 클래스
struct cLobby : public iScene
{
	//로비 메세지
	char* m_szLobby;
public:
	cLobby();
	~cLobby();

	virtual void Init();
	virtual void Update();
	virtual void Render();
};

