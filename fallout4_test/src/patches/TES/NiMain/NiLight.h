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

#include "NiColor.h"
#include "NiAVObject.h"

class NiLight : public NiAVObject
{
public:
	NiColor m_kAmb;
	NiColor m_kDiff;
	NiColor m_kSpec;
	float m_fDimmer;
	char _pad2[0x4];

public:
	const NiColor& GetAmbientColor() const
	{
		return m_kAmb;
	}

	const NiColor& GetDiffuseColor() const
	{
		return m_kDiff;
	}

	const NiColor& GetSpecularColor() const
	{
		return m_kSpec;
	}

	float GetDimmer() const
	{
		return m_fDimmer;
	}
};
static_assert(sizeof(NiLight) == 0x140);
static_assert_offset(NiLight, m_kAmb, 0x110);
static_assert_offset(NiLight, m_kDiff, 0x11C);
static_assert_offset(NiLight, m_kSpec, 0x128);
static_assert_offset(NiLight, m_fDimmer, 0x134);