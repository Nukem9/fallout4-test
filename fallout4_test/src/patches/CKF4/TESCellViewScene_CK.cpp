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

/*

00007FF79650CB00 | C781 38030000 00002048       | mov dword ptr ds:[rcx+338],48200000                                 |
00007FF79650CB0A | C781 3C030000 40002048       | mov dword ptr ds:[rcx+33C],48200040                                 |
00007FF79650CB14 | 48:C781 4C030000 00409C46    | mov qword ptr ds:[rcx+34C],469C4000                                 |
00007FF79650CB1F | C781 54030000 00409C46       | mov dword ptr ds:[rcx+354],469C4000                                 |
00007FF79650CB29 | 33C0                         | xor eax,eax                                                         |
00007FF79650CB2B | 8981 48030000                | mov dword ptr ds:[rcx+348],eax                                      |
00007FF79650CB31 | C781 5C030000 0000803F       | mov dword ptr ds:[rcx+35C],3F800000                                 |
00007FF79650CB3B | C781 60030000 0000803F       | mov dword ptr ds:[rcx+360],3F800000                                 |
00007FF79650CB45 | C781 64030000 0000803F       | mov dword ptr ds:[rcx+364],3F800000                                 |

*/


BOOL TESCellViewSceneRenderInfo_CK::IsSky(VOID) const
{
	BYTE flag = *(BYTE*)(((uintptr_t)this) + 0x37C);
	// 
	// 37C == 1 or 3
	return (flag == 1) || (flag == 3);
}


/*
==================
TESCellViewScene_CK::GetCellViewScene

Returns the global view scene (rav+0x6D54CF8)
==================
*/
TESCellViewScene_CK* TESCellViewScene_CK::GetCellViewScene(VOID)
{
	return *(TESCellViewScene_CK**)(OFFSET(0x6D54CF8, 0));
}


/*
==================
TESCellViewScene_CK::IsEmpty

When initialized 0x48 0x4C 0x50 0x54 are equal to 7FFFFFFF
==================
*/
BOOL TESCellViewScene_CK::IsEmpty(VOID) const
{
	return (*(PDWORD)(_pad1 + 0x48) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x4C) == 0x7FFFFFFF) &&
		(*(PDWORD)(_pad1 + 0x50) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x54) == 0x7FFFFFFF);
}