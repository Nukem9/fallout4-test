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

#include "NiMatrix.h"

class NiTransform
{
public:
	NiMatrix3 m_Rotate;
	NiPoint3 m_Translate;
	float m_fScale;

	inline NiTransform()
	{
		m_fScale = 1.0f;
	}

	inline NiTransform(const NiTransform& Src) : m_Rotate(Src.m_Rotate), m_Translate(Src.m_Translate), m_fScale(Src.m_fScale)
	{
	}

	void Invert(NiTransform& Dest) const
	{
		Dest.m_Rotate = m_Rotate.Transpose();
		Dest.m_fScale = 1.0f / m_fScale;
		Dest.m_Translate = Dest.m_fScale * (Dest.m_Rotate * -m_Translate);
	}

	inline NiTransform operator* (const NiTransform &xform) const
	{
		NiTransform res;

		res.m_fScale = m_fScale * xform.m_fScale;
		res.m_Rotate = m_Rotate * xform.m_Rotate;
		res.m_Translate = m_Translate + m_fScale * (m_Rotate * xform.m_Translate);

		return res;
	}
};
static_assert(sizeof(NiTransform) == 0x34);
static_assert_offset(NiTransform, m_Rotate, 0x0);
static_assert_offset(NiTransform, m_Translate, 0x24);
static_assert_offset(NiTransform, m_fScale, 0x30);