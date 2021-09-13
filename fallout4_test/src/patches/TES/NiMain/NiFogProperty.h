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
#include "NiColor.h"

class NiFogProperty : public NiObjectNET
{
public:
	virtual ~NiFogProperty();

	uint16_t m_uFlags;
	float m_fDepth;
	NiColor m_kNearColor;
	NiColor m_kFarColor;
};
static_assert(sizeof(NiFogProperty) == 0x50);
static_assert_offset(NiFogProperty, m_uFlags, 0x30);
static_assert_offset(NiFogProperty, m_fDepth, 0x34);
static_assert_offset(NiFogProperty, m_kNearColor, 0x38);
static_assert_offset(NiFogProperty, m_kFarColor, 0x44);

class BSFogProperty : public NiFogProperty
{
public:
	virtual ~BSFogProperty();

	float fStartDistance;
	float fEndDistance;
	float fStartWaterDistance;
	float fEndWaterDistance;
	float kPlane[4];// NiPlane
	float fFalloff;
	float fHeight;
	NiColor kWaterColor;
	float fPower;
	float fClamp;
};
static_assert(sizeof(BSFogProperty) == 0x90);
static_assert_offset(BSFogProperty, fStartDistance, 0x50);
static_assert_offset(BSFogProperty, fEndDistance, 0x54);
static_assert_offset(BSFogProperty, fStartWaterDistance, 0x58);
static_assert_offset(BSFogProperty, fEndWaterDistance, 0x5C);
static_assert_offset(BSFogProperty, kPlane, 0x60);
static_assert_offset(BSFogProperty, fFalloff, 0x70);
static_assert_offset(BSFogProperty, fHeight, 0x74);
static_assert_offset(BSFogProperty, kWaterColor, 0x78);
static_assert_offset(BSFogProperty, fPower, 0x84);
static_assert_offset(BSFogProperty, fClamp, 0x88);