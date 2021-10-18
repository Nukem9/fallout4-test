//////////////////////////////////////////
/*
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

#include "TESCellViewScene_CK.h"

BOOL TESCellViewScene_CK::TESRenderInfo_CK::IsSky(VOID) const
{
	BYTE flag = *(BYTE*)(((uintptr_t)this) + 0x37C);
	// 
	// 37C == 1 or 3
	return (flag == 1) || (flag == 3);
}


TESCellViewScene_CK* TESCellViewScene_CK::GetCellViewScene(VOID)
{
	return *(TESCellViewScene_CK**)(OFFSET(0x6D54CF8, 0));
}


BOOL TESCellViewScene_CK::IsEmpty(VOID) const
{
	return (*(PDWORD)(_pad1 + 0x48) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x4C) == 0x7FFFFFFF) &&
		(*(PDWORD)(_pad1 + 0x50) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x54) == 0x7FFFFFFF);
}