// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <thread>
#include <vector>
#include <string>
#include <ctime>
#include "util.h"

using namespace std;


//함수 매크로
#define PROPERTY(var_type, var_name, property_name)\
protected:\
var_type var_name;\
public:\
var_type Get##property_name(){return var_name;}\
void Set##property_name(var_type var){var_name = var;}

//벡터2 정의
struct Vecter2
{
	int m_nX, m_nY = 0;
	Vecter2() {}
	Vecter2(int nX, int nY)
		:m_nX(nX)
		,m_nY(nY)
	{

	}

	void Normalize()
	{
		if (m_nX > 0)
		{
			m_nX = 1;
			m_nY = 0;
		}
		else if (m_nX < 0)
		{
			m_nX = -1;
			m_nY = 0;
		}
		else if (m_nY > 0)
		{
			m_nX = 0;
			m_nY = 1;
		}
		else if (m_nY < 0)
		{
			m_nX = 0;
			m_nY = -1;
		}
	}
};

//벡터2 연산자 오버로딩
inline bool operator== (Vecter2 vPos1, Vecter2 vPos2)
{
	if (vPos1.m_nX == vPos2.m_nX && vPos1.m_nY == vPos2.m_nY)
		return true;
	else
		return false;
}
//벡터2 연산자 오버로딩
inline bool operator!= (Vecter2 vPos1, Vecter2 vPos2)
{
	if (!(vPos1 == vPos2))
		return true;
	else
		return false;
}
//벡터2 연산자 오버로딩
inline Vecter2 operator+ (Vecter2 vPos1, Vecter2 vPos2)
{
	return Vecter2(vPos1.m_nX + vPos2.m_nX, vPos1.m_nY + vPos2.m_nY);
}
//벡터2 연산자 오버로딩
inline Vecter2 operator* (int fValue, Vecter2 vPos1)
{
	return Vecter2(vPos1.m_nX * fValue, vPos1.m_nY * fValue);
}
//벡터2 연산자 오버로딩
inline Vecter2 operator- (Vecter2 vPos1, Vecter2 vPos2)
{
	return vPos1 + (-1 * vPos2);
}

//오브젝트 타입 enum문
enum eObjectType
{
	E_EMPTY = 0,
	E_WALL,
	E_FINISH,
	E_ENEMY,
	E_PLAYER
};

//오브젝트 구조체
struct ST_OBJECT
{
public:
	//타입
	eObjectType m_eType;
	//위치
	Vecter2 m_vPos;

	ST_OBJECT() {}

	ST_OBJECT(eObjectType type, Vecter2 vPos)
		: m_eType(type)
		, m_vPos(vPos)
	{

	}
	//타입에 대한 렌더링 함수
	virtual void Render()
	{
		switch (m_eType)
		{
		case E_EMPTY:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "□");
			break;
		case E_WALL:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "▦");
			break;
		case E_FINISH:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "▒");
			break;
		case E_ENEMY:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "◆");
			break;
		case E_PLAYER:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "◎");
			break;
		}
	}
};

//오브젝트를 상속받은 ENEMY 구조체
struct ST_ENEMY : public ST_OBJECT
{
	//장애물의 목적지 배열
	vector<Vecter2> m_vecDest;
	//방향벡터
	Vecter2 m_vDirection = Vecter2(0, 0);
	//목적지 인덱스
	int m_nIndex = 0;
	//몇번의 업데이트당 움직일건지 결정
	int m_nMaxCount = 10;
	int m_nCount = m_nMaxCount;

	ST_ENEMY(eObjectType type, int nIndex)
	{
		m_nIndex = nIndex;
		m_eType = type;
	}

	//이동 매커니즘
	void Update()
	{
		//현재 목적지에 도착하면 목적지 인덱스를 바꿔주는 로직
		if (m_vecDest[m_nIndex] == m_vPos)
		{
			m_nIndex = (m_nIndex + 1) % m_vecDest.size();
			m_vDirection = m_vecDest[m_nIndex] - m_vPos;
			m_vDirection.Normalize();
		}

		//이동 로직
		if (--m_nCount == 0)
		{
			m_nCount = m_nMaxCount;
			m_vPos = m_vPos + m_vDirection;
		}

	}
};

struct ST_PLAYER : public ST_OBJECT
{
	//몇번의 업데이트당 움직일건지 결정
	int m_nMaxCount = 5;
	int m_nCount = m_nMaxCount;

	ST_PLAYER(eObjectType type, Vecter2 vPos)
		: ST_OBJECT(type, vPos)
	{
	}

	//업데이트 가능 체크
	bool Update()
	{
		//입력 프레임 스킵
		if (--m_nCount == 0)
		{
			m_nCount = m_nMaxCount;
			return true;
		}
		return false;
	}
};

extern bool g_isQuit;


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
