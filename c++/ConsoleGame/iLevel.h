#pragma once

//레벨 딜리게이트를 위한 추상클래스(프로그래밍 구조를 짤때 클래스의 단방향성을 위해 사용됨)
struct iLevelDelegate
{
public:
	virtual void OnLevelFinished() = 0;
};

//레벨에 대한 추상클래스
struct iLevel
{
protected:
	//레벨 인덱스(나중에 이 인덱스에 해당하는 TXT 파일을 파싱한다.)
	PROPERTY(DWORD, m_dwLevelIndex, LevelIndex);
	//레벨 딜리게이트(콜백을 보낼 인스턴스 저장)
	PROPERTY(iLevelDelegate*, m_pDelegate, Delegate);
public:
	iLevel();
	~iLevel();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

