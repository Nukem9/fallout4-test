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

class NiRefObject
{
private:
	inline AutoPtr(uint32_t, ms_uiObjects, 0x3038520);

protected:
	uint32_t m_uiRefCount;

public:
	NiRefObject() : m_uiRefCount(0)
	{
		InterlockedIncrement(&ms_uiObjects);
	}

	virtual ~NiRefObject()
	{
		InterlockedDecrement(&ms_uiObjects);
	}

	virtual void DeleteThis()
	{
		if (this)
			this->~NiRefObject();
	}

	uint32_t IncRefCount()
	{
		return InterlockedIncrement(&m_uiRefCount);
	}

	uint32_t DecRefCount()
	{
		uint32_t count = InterlockedDecrement(&m_uiRefCount);

		if (count <= 0)
			DeleteThis();

		return count;
	}

	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		Callback("-- NiRefObject --\n");
		Callback("This = 0x%p\n", this);
		Callback("Ref Count = %u\n", m_uiRefCount);
	}

	static uint32_t GetTotalObjectCount()
	{
		return ms_uiObjects;
	}
};
static_assert(sizeof(NiRefObject) == 0x10);
//static_assert_offset(NiRefObject, m_uiRefCount, 0x8);