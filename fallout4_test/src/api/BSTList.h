//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

template<typename T>
class BSSimpleList
{
public:
	using BSSimpleListNode = BSSimpleList<T>;
	using BSSimpleListNodePtr = BSSimpleListNode*;
public:
	T m_item;
	BSSimpleListNodePtr m_pkNext;

	BSSimpleList()
	{
		m_item = (T)0;
		m_pkNext = nullptr;
	}

	~BSSimpleList()
	{
		RemoveAllNodes(nullptr, nullptr);
	}

	BSSimpleListNodePtr AddNode(BSSimpleListNodePtr Node, T& Item)
	{
		BSSimpleListNodePtr NewNode = new BSSimpleListNode();
		NewNode->m_item = Item;

		if (Node->m_pkNext)
		{
			NewNode->m_pkNext = Node->m_pkNext;
			Node->m_pkNext = NewNode;
		}
		else
		{
			Node->m_pkNext = NewNode;
		}

		return NewNode;
	}

	static void operator delete(void *Ptr, std::size_t Size)
	{
		AutoFunc(void(__fastcall *)(void *), sub_1401026F0, 0x1026F0);
		sub_1401026F0((void *)Ptr);
	}

	void RemoveNode(void(*Callback)(BSSimpleList<T> *, void *) = nullptr, void *UserData = nullptr)
	{
		BSSimpleList<T> *currNode = m_pkNext;

		if (currNode)
		{
			m_pkNext = currNode->m_pkNext;
			m_item = currNode->m_item;
			currNode->m_pkNext = nullptr;

			if (Callback)
				Callback(currNode, UserData);
			else
				delete currNode;
		}
		else
		{
			m_item = (T)0;
		}
	}

	void RemoveAllNodes(void(*Callback)(BSSimpleList<T> *, void *) = nullptr, void *UserData = nullptr)
	{
		if (m_pkNext)
		{
			while (true)
			{
				BSSimpleList<T> *v6 = this->m_pkNext;
				BSSimpleList<T> *v7 = v6->m_pkNext;
				v6->m_pkNext = nullptr;

				if (Callback)
					break;

				BSSimpleList<T> *v8 = this->m_pkNext;

				if (v8)
				{
					delete v8;
					goto LABEL_6;
				}

			LABEL_7:
				this->m_pkNext = v7;

				if (!v7)
				{
					this->m_item = (T)0;
					return;
				}
			}

			Callback(this->m_pkNext, UserData);

		LABEL_6:
			goto LABEL_7;
		}

		this->m_item = (T)0;
	}

	T QItem() const
	{
		return m_item;
	}

	BSSimpleList<T> *QNext() const
	{
		return m_pkNext;
	}
};