// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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


//�Լ� ��ũ��
#define PROPERTY(var_type, var_name, property_name)\
protected:\
var_type var_name;\
public:\
var_type Get##property_name(){return var_name;}\
void Set##property_name(var_type var){var_name = var;}

//����2 ����
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

//����2 ������ �����ε�
inline bool operator== (Vecter2 vPos1, Vecter2 vPos2)
{
	if (vPos1.m_nX == vPos2.m_nX && vPos1.m_nY == vPos2.m_nY)
		return true;
	else
		return false;
}
//����2 ������ �����ε�
inline bool operator!= (Vecter2 vPos1, Vecter2 vPos2)
{
	if (!(vPos1 == vPos2))
		return true;
	else
		return false;
}
//����2 ������ �����ε�
inline Vecter2 operator+ (Vecter2 vPos1, Vecter2 vPos2)
{
	return Vecter2(vPos1.m_nX + vPos2.m_nX, vPos1.m_nY + vPos2.m_nY);
}
//����2 ������ �����ε�
inline Vecter2 operator* (int fValue, Vecter2 vPos1)
{
	return Vecter2(vPos1.m_nX * fValue, vPos1.m_nY * fValue);
}
//����2 ������ �����ε�
inline Vecter2 operator- (Vecter2 vPos1, Vecter2 vPos2)
{
	return vPos1 + (-1 * vPos2);
}

//������Ʈ Ÿ�� enum��
enum eObjectType
{
	E_EMPTY = 0,
	E_WALL,
	E_FINISH,
	E_ENEMY,
	E_PLAYER
};

//������Ʈ ����ü
struct ST_OBJECT
{
public:
	//Ÿ��
	eObjectType m_eType;
	//��ġ
	Vecter2 m_vPos;

	ST_OBJECT() {}

	ST_OBJECT(eObjectType type, Vecter2 vPos)
		: m_eType(type)
		, m_vPos(vPos)
	{

	}
	//Ÿ�Կ� ���� ������ �Լ�
	virtual void Render()
	{
		switch (m_eType)
		{
		case E_EMPTY:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "��");
			break;
		case E_WALL:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "��");
			break;
		case E_FINISH:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "��");
			break;
		case E_ENEMY:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "��");
			break;
		case E_PLAYER:
			ScreenPrint(m_vPos.m_nX * 2, m_vPos.m_nY, "��");
			break;
		}
	}
};

//������Ʈ�� ��ӹ��� ENEMY ����ü
struct ST_ENEMY : public ST_OBJECT
{
	//��ֹ��� ������ �迭
	vector<Vecter2> m_vecDest;
	//���⺤��
	Vecter2 m_vDirection = Vecter2(0, 0);
	//������ �ε���
	int m_nIndex = 0;
	//����� ������Ʈ�� �����ϰ��� ����
	int m_nMaxCount = 10;
	int m_nCount = m_nMaxCount;

	ST_ENEMY(eObjectType type, int nIndex)
	{
		m_nIndex = nIndex;
		m_eType = type;
	}

	//�̵� ��Ŀ����
	void Update()
	{
		//���� �������� �����ϸ� ������ �ε����� �ٲ��ִ� ����
		if (m_vecDest[m_nIndex] == m_vPos)
		{
			m_nIndex = (m_nIndex + 1) % m_vecDest.size();
			m_vDirection = m_vecDest[m_nIndex] - m_vPos;
			m_vDirection.Normalize();
		}

		//�̵� ����
		if (--m_nCount == 0)
		{
			m_nCount = m_nMaxCount;
			m_vPos = m_vPos + m_vDirection;
		}

	}
};

struct ST_PLAYER : public ST_OBJECT
{
	//����� ������Ʈ�� �����ϰ��� ����
	int m_nMaxCount = 5;
	int m_nCount = m_nMaxCount;

	ST_PLAYER(eObjectType type, Vecter2 vPos)
		: ST_OBJECT(type, vPos)
	{
	}

	//������Ʈ ���� üũ
	bool Update()
	{
		//�Է� ������ ��ŵ
		if (--m_nCount == 0)
		{
			m_nCount = m_nMaxCount;
			return true;
		}
		return false;
	}
};

extern bool g_isQuit;


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
