#pragma once
#include "stdafx.h"

template<class T>
class cStack
{
protected:
	class cInnerNode
	{
	public:
		T m_Data;
		cInnerNode* m_NextNode;

		cInnerNode() : m_NextNode(0) {}
	};

	cInnerNode* m_Top;
	size_t m_Size;
public:
	cStack();
	~cStack();

	void Push(T item);
	const T Pop();
	const size_t Length() const;
	const T Peek() const;

	//Debug Function
	void PrintNodes();
};

template<class T>
inline cStack<T>::cStack()
	: m_Top(0)
	, m_Size(0)
{
	
}

template<class T>
inline cStack<T>::~cStack()
{
	cInnerNode* currentNode = m_Top;
	while (currentNode != 0)
	{
		cInnerNode* tempNode = currentNode;
		currentNode = currentNode->m_NextNode;
		delete tempNode;
	}
}

template<class T>
inline void cStack<T>::Push(T item)
{
	cInnerNode* newNode = new cInnerNode;
	newNode->m_Data = item;
	newNode->m_NextNode = m_Top;
	m_Top = newNode;
	++m_Size;
}

template<class T>
inline const T cStack<T>::Pop()
{
	T popItem = m_Top->m_Data;
	cInnerNode* popNode = m_Top;
	m_Top = m_Top->m_NextNode;
	m_Size ? --m_Size : m_Size = 0;
	delete popNode;
	return popItem;
}

template<class T>
inline const size_t cStack<T>::Length() const
{
	return m_Size;
}

template<class T>
inline const T cStack<T>::Peek() const
{
	return m_Top->m_Data;
}

template<class T>
inline void cStack<T>::PrintNodes()
{

	// only print console data structure
	cInnerNode* currentNode = m_Top;
	while (currentNode != 0)
	{
		cout << currentNode->m_Data << endl;
		currentNode = currentNode->m_NextNode;
	}
}
