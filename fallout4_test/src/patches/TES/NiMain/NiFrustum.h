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

struct NiFrustum
{
	float m_fLeft;
	float m_fRight;
	float m_fTop;
	float m_fBottom;
	float m_fNear;
	float m_fFar;
	bool m_bOrtho;
};
static_assert_offset(NiFrustum, m_fLeft, 0x0);
static_assert_offset(NiFrustum, m_fRight, 0x4);
static_assert_offset(NiFrustum, m_fTop, 0x8);
static_assert_offset(NiFrustum, m_fBottom, 0xC);
static_assert_offset(NiFrustum, m_fNear, 0x10);
static_assert_offset(NiFrustum, m_fFar, 0x14);
static_assert_offset(NiFrustum, m_bOrtho, 0x18);