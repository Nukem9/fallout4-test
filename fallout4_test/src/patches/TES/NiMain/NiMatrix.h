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

#include "NiPoint.h"

class NiMatrix3
{
public:
	float m_pEntry[3][3];

	inline NiMatrix3()
	{
	}

	inline NiMatrix3(const NiMatrix3& Src)
	{
		memcpy(&m_pEntry, Src.m_pEntry, sizeof(m_pEntry));
	}

	template<uint32_t Index>
	NiPoint3 GetCol() const
	{
		static_assert(Index <= 2);

		return NiPoint3(m_pEntry[0][Index], m_pEntry[1][Index], m_pEntry[2][Index]);
	}

	NiMatrix3 Transpose() const
	{
		// Swap [rows, cols] with [cols, rows]. Can only be optimized with AVX.
		NiMatrix3 m;

		m.m_pEntry[0][0] = m_pEntry[0][0];
		m.m_pEntry[0][1] = m_pEntry[1][0];
		m.m_pEntry[0][2] = m_pEntry[2][0];

		m.m_pEntry[1][0] = m_pEntry[0][1];
		m.m_pEntry[1][1] = m_pEntry[1][1];
		m.m_pEntry[1][2] = m_pEntry[2][1];

		m.m_pEntry[2][0] = m_pEntry[0][2];
		m.m_pEntry[2][1] = m_pEntry[1][2];
		m.m_pEntry[2][2] = m_pEntry[2][2];

		return m;
	}

	NiMatrix3 NiMatrix3::operator* (const NiMatrix3& mat) const
	{
		NiMatrix3 prd;

		prd.m_pEntry[0][0] =
			m_pEntry[0][0] * mat.m_pEntry[0][0] +
			m_pEntry[0][1] * mat.m_pEntry[1][0] +
			m_pEntry[0][2] * mat.m_pEntry[2][0];
		prd.m_pEntry[1][0] =
			m_pEntry[1][0] * mat.m_pEntry[0][0] +
			m_pEntry[1][1] * mat.m_pEntry[1][0] +
			m_pEntry[1][2] * mat.m_pEntry[2][0];
		prd.m_pEntry[2][0] =
			m_pEntry[2][0] * mat.m_pEntry[0][0] +
			m_pEntry[2][1] * mat.m_pEntry[1][0] +
			m_pEntry[2][2] * mat.m_pEntry[2][0];
		prd.m_pEntry[0][1] =
			m_pEntry[0][0] * mat.m_pEntry[0][1] +
			m_pEntry[0][1] * mat.m_pEntry[1][1] +
			m_pEntry[0][2] * mat.m_pEntry[2][1];
		prd.m_pEntry[1][1] =
			m_pEntry[1][0] * mat.m_pEntry[0][1] +
			m_pEntry[1][1] * mat.m_pEntry[1][1] +
			m_pEntry[1][2] * mat.m_pEntry[2][1];
		prd.m_pEntry[2][1] =
			m_pEntry[2][0] * mat.m_pEntry[0][1] +
			m_pEntry[2][1] * mat.m_pEntry[1][1] +
			m_pEntry[2][2] * mat.m_pEntry[2][1];
		prd.m_pEntry[0][2] =
			m_pEntry[0][0] * mat.m_pEntry[0][2] +
			m_pEntry[0][1] * mat.m_pEntry[1][2] +
			m_pEntry[0][2] * mat.m_pEntry[2][2];
		prd.m_pEntry[1][2] =
			m_pEntry[1][0] * mat.m_pEntry[0][2] +
			m_pEntry[1][1] * mat.m_pEntry[1][2] +
			m_pEntry[1][2] * mat.m_pEntry[2][2];
		prd.m_pEntry[2][2] =
			m_pEntry[2][0] * mat.m_pEntry[0][2] +
			m_pEntry[2][1] * mat.m_pEntry[1][2] +
			m_pEntry[2][2] * mat.m_pEntry[2][2];

		return prd;
	}

	NiPoint3 operator* (const NiPoint3& Point) const
	{
		return NiPoint3
		(
			m_pEntry[0][0] * Point.x + m_pEntry[0][1] * Point.y + m_pEntry[0][2] * Point.z,
			m_pEntry[1][0] * Point.x + m_pEntry[1][1] * Point.y + m_pEntry[1][2] * Point.z,
			m_pEntry[2][0] * Point.x + m_pEntry[2][1] * Point.y + m_pEntry[2][2] * Point.z
		);
	}
};
static_assert(sizeof(NiMatrix3) == 0x24);
static_assert_offset(NiMatrix3, m_pEntry, 0x0);

inline NiPoint3 operator*(const NiPoint3& Point, const NiMatrix3& Mat)
{
	return NiPoint3
	(
		Point.x * Mat.m_pEntry[0][0] + Point.y * Mat.m_pEntry[1][0] +
		Point.z * Mat.m_pEntry[2][0],
		Point.x * Mat.m_pEntry[0][1] + Point.y * Mat.m_pEntry[1][1] +
		Point.z * Mat.m_pEntry[2][1],
		Point.x * Mat.m_pEntry[0][2] + Point.y * Mat.m_pEntry[1][2] +
		Point.z * Mat.m_pEntry[2][2]
	);
}