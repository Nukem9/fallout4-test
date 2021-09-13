//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
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

#include "../../common.h"
#include "../TES/BSTArray.h"

class TESForm_CK
{
private:
	CHAR _pad[0x8];
	UINT32 FormFlags;
	UINT32 FormID;
public:
	inline BOOL GetActive(VOID) const { return (FormFlags & 2) != 0; }
	inline uint32_t GetFormID(VOID) const { return FormID; }
	std::string GetEditID(VOID) const;
public:
	READ_PROPERTY(GetActive) BOOL Active;
	READ_PROPERTY(GetFormID) UINT32 FormID;
	READ_PROPERTY(GetEditID) std::string EditID;
public:
	using Array = BSTArray<TESForm_CK*>;

	virtual ~TESForm_CK();
	static TESForm_CK* GetFormByNumericID(UINT32 SearchID);
};