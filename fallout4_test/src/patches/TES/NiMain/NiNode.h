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

#include "NiAVObject.h"

template<typename T>
class NiTArray
{
public:
	virtual ~NiTArray();

	T *m_pBase;				// Sparse array (nullptr for invalid)
	uint16_t m_usMaxSize;	// Number of elements allocated
	uint16_t m_usSize;		// Last index with a valid element
	uint16_t m_usESize;		// Number of elements in use
	uint16_t m_usGrowBy;

	uint16_t GetMaxSize() const
	{
		return m_usMaxSize;
	}

	uint16_t GetSize() const
	{
		return m_usSize;
	}

	uint16_t GetESize() const
	{
		return m_usESize;
	}

	T At(size_t Index) const
	{
		return m_pBase[Index];
	}
};

template<typename T>
class NiTObjectArray : public NiTArray<T>
{
public:
	virtual ~NiTObjectArray();
};

class NiNode : public NiAVObject
{
public:
	NiTObjectArray<NiAVObject *> m_kChildren;

	uint32_t GetMaxSize() const
	{
		return m_kChildren.GetMaxSize();
	}

	uint32_t GetArrayCount() const
	{
		return m_kChildren.GetSize();
	}

	uint32_t GetChildCount() const
	{
		return m_kChildren.GetESize();
	}

	NiAVObject *GetAt(size_t Index) const
	{
		return m_kChildren.At(Index);
	}

	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiNode --\n");
		Callback("Child Count = %d\n", GetChildCount());
	}
};
static_assert(sizeof(NiNode) == 0x128);
static_assert_offset(NiNode, m_kChildren, 0x110);