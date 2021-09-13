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

#include <xmmintrin.h>

class NiColor
{
public:
	const static NiColor BLACK;
	const static NiColor WHITE;

	float r;
	float g;
	float b;

	inline NiColor(float initR = 0.0f, float initG = 0.0f, float initB = 0.0f)
	{
		Set(initR, initG, initB);
	}

	inline void Set(float R = 0.0f, float G = 0.0f, float B = 0.0f)
	{
		r = R;
		g = G;
		b = B;
	}
};

class NiColorA
{
public:
	const static NiColorA BLACK;
	const static NiColorA WHITE;
	const static NiColorA RED;
	const static NiColorA GREEN;
	const static NiColorA BLUE;

	float r;
	float g;
	float b;
	float a;

	inline NiColorA(float initR = 0.0f, float initG = 0.0f, float initB = 0.0f, float initA = 0.0f)
	{
		Set(initR, initG, initB, initA);
	}

	inline NiColorA(const NiColor& Src, float A)
	{
		Set(Src.r, Src.g, Src.b, A);
	}

	inline void Set(float R = 0.0f, float G = 0.0f, float B = 0.0f, float A = 0.0f)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	}

	inline const float *Data() const
	{
		return &r;
	}

	inline __m128 AsXmm() const
	{
		return _mm_load_ps(Data());
	}
};
static_assert(sizeof(NiColorA) == 0x10);
static_assert_offset(NiColorA, r, 0x0);
static_assert_offset(NiColorA, g, 0x4);
static_assert_offset(NiColorA, b, 0x8);
static_assert_offset(NiColorA, a, 0xC);