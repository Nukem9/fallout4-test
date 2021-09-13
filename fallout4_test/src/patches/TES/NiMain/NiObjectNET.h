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

#include "NiObject.h"

struct BSFixedString
{
	void *ptr;

	BSFixedString(const char *String)
	{
		AutoFunc(BSFixedString *(*)(BSFixedString&, const char *), sub_140C28280, 0xC28280);
		sub_140C28280(*this, String);
	}

	~BSFixedString()
	{
		AutoFunc(void(*)(BSFixedString&), sub_140C283D0, 0xC283D0);
		sub_140C283D0(*this);
	}

	BSFixedString& operator= (BSFixedString &Other)
	{
		AutoFunc(BSFixedString&(*)(BSFixedString&, BSFixedString&), sub_140C284B0, 0xC284B0);
		return sub_140C284B0(*this, Other);
	}

	const char *c_str() const
	{
		return (const char *)ptr;
	}
};

class NiObjectNET : public NiObject
{
public:
	virtual ~NiObjectNET();

	BSFixedString m_kName;
	char _pad[0x18];

	const BSFixedString *GetName() const
	{
		return &m_kName;
	}

	void SetName(BSFixedString& Name)
	{
		m_kName = Name;
	}

	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiObjectNET --\n");
		Callback("Name = %s\n", GetName()->c_str());
	}
};
static_assert(sizeof(NiObjectNET) == 0x30);
static_assert_offset(NiObjectNET, m_kName, 0x10);