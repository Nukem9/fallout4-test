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

#include "NiRefObject.h"

template<typename T>
class NiPointer
{
protected:
	T *m_pObject;

public:
	//static_assert(std::is_base_of_v<NiRefObject, T>, "T must inherit NiRefObject");

	inline NiPointer(T *Object = (T *)nullptr)
	{
		m_pObject = Object;

		if (m_pObject)
			m_pObject->IncRefCount();
	}

	inline NiPointer(const NiPointer<T>& Other)
	{
		m_pObject = Other.m_pObject;

		if (m_pObject)
			m_pObject->IncRefCount();
	}

	inline ~NiPointer()
	{
		if (m_pObject)
			m_pObject->DecRefCount();
	}

	inline operator T*() const
	{
		return m_pObject;
	}

	inline T& operator*() const
	{
		return m_pObject;
	}

	inline T* operator->() const
	{
		return m_pObject;
	}

	inline NiPointer<T>& operator=(T *Other)
	{
		if (m_pObject != Other)
		{
			if (Other)
				Other->IncRefCount();

			if (m_pObject)
				m_pObject->DecRefCount();

			m_pObject = Other;
		}

		return *this;
	}

	inline bool operator!=(const T *Other) const
	{
		return m_pObject != Other;
	}
};
static_assert(sizeof(NiPointer<NiRefObject>) == 0x8);