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

#include "NiObjectNET.h"

class NiProperty : public NiObjectNET
{
public:
	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiProperty --\n");
	}
};
static_assert(sizeof(NiProperty) == 0x30);

class NiShadeProperty : public NiProperty
{
public:
	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiShadeProperty --\n");
	}
};
static_assert(sizeof(NiProperty) == 0x30);

class NiAlphaProperty : public NiProperty
{
private:
	union
	{
		uint16_t Value;
		struct
		{
			uint16_t DoBlending : 1;
			uint16_t SrcBlend : 4;	// BlendFunction
			uint16_t DstBlend : 4;	// BlendFunction
			uint16_t DoTesting : 1;
			uint16_t TestFunc : 3;	// TestFunction
			uint16_t NoSorter : 1;
		};
	} AlphaFlags;
	uint8_t m_ucAlphaTestRef;

public:
	bool GetAlphaBlending() const
	{
		return AlphaFlags.DoBlending;
	}

	bool GetAlphaTesting() const
	{
		return AlphaFlags.DoTesting;
	}

	uint8_t GetTestRef() const
	{
		return m_ucAlphaTestRef;
	}

	void GetViewerStrings(void(*Callback)(const char *, ...), bool Recursive) const
	{
		if (Recursive)
			__super::GetViewerStrings(Callback, Recursive);

		Callback("-- NiAlphaProperty --\n");
		Callback("Alpha Blend: %s\n", GetAlphaBlending() ? "true" : "false");
		Callback("Alpha Test: %s\n", GetAlphaTesting() ? "true" : "false");
		Callback("Test Ref: %u\n", (uint32_t)GetTestRef());
	}
};
static_assert(sizeof(NiAlphaProperty) == 0x38);
//static_assert_offset(NiAlphaProperty, AlphaFlags, 0x30);
//static_assert_offset(NiAlphaProperty, m_ucAlphaTestRef, 0x32);