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

/*
* There will be patches from the author here SeargeDP.
* Profile: http://www.nexusmods.com/users/5780168
* The file that is patched is too large and it has an unpleasant effect on the overall work of CK, 
* some fixes that he implemented will be here.
* 
* YES, IT'S NOT TOO GOOD, BUT I'LL POINT HIM OUT AS THE CREATOR OF THE IDEAS THAT HE INVESTED.
* AS I SAID ABOVE, NOT ALL PATCHES WILL BE HERE, DUE TO THE FACT THAT SOME FIXES MAY AFFECT 
* THE OVERALL WORK OF THE PROGRAM ON OTHER TASKS. FIXES WILL BE SELECTED BY CONDUCTING 
* A COMPARATIVE ANALYSIS.
*/

#include "../../StdAfx.h"

typedef DWORD (*FIXAPI fn_59C160_t)(LPVOID, DWORD);
fn_59C160_t call_59C160;

typedef VOID(*FIXAPI fn_59C1D0_t)(LPVOID, DWORD, BOOL);
fn_59C1D0_t call_59C1D0;

typedef INT64(*FIXAPI fn_107C90_t)(LPVOID, LPVOID);
fn_107C90_t call_107C90;

typedef LPVOID(*FIXAPI fn_247CD60_t)(LPVOID, LPVOID);
fn_247CD60_t call_247CD60;

DWORD FIXAPI fn_59C160(LPVOID UnkClass, DWORD UnkParam) {
	// interesting hmmm... why 0x1000
	// This is very unusual, I can see from the code that this is a class
	// The pointer to the existing pointer is of course larger 0x1000, but why not NULL?
	//if (UnkClass < (LPVOID)0x1000)

	// I find this nonsense, but there really is no protection, I will check for the existence of the object
	if (!UnkClass)
		return 0;

	return call_59C160(UnkClass, UnkParam);
}

VOID FIXAPI fn_59C1D0(LPVOID UnkClass, DWORD UnkParam, BOOL UnkBool) {
	// This has nothing to do with the author SeargeDP, I added this yourself, because I see the Set() function in this
	if (!UnkClass)
		return;

	call_59C1D0(UnkClass, UnkParam, UnkBool);
}

INT64 FIXAPI fn_107C90(LPVOID UnkClass, LPVOID UnkParam) {
	// The same 0x1000, I set NULL compare this.
	// Also need compare UnkParam
	if (!UnkClass || !UnkParam)
		return 0;

	return call_107C90(UnkClass, UnkParam);
}

LPVOID FIXAPI fn_247CD60(LPVOID UnkClass, LPVOID UnkParam) {
	// The same 0x1000, I set NULL compare this.
	// Also need compare UnkParam
	// The grossest mistake of the author SeargeDP, the function does not change rax
	if (!UnkClass || !UnkParam)
		return NULL;

	return call_247CD60(UnkClass, UnkParam);
}

/* MAIN PATCH */
VOID FIXAPI Fix_PatchSeargeDPPreCombined(VOID) {
	*(uintptr_t*)&call_59C160 = Detours::X64::DetourFunctionClass(OFFSET(0x59C160, 0), &fn_59C160);	// Get
	*(uintptr_t*)&call_59C1D0 = Detours::X64::DetourFunctionClass(OFFSET(0x59C1D0, 0), &fn_59C1D0);	// Set
	*(uintptr_t*)&call_107C90 = Detours::X64::DetourFunctionClass(OFFSET(0x107C90, 0), &fn_107C90);
	*(uintptr_t*)&call_247CD60 = Detours::X64::DetourFunctionClass(OFFSET(0x247CD60, 0), &fn_247CD60);
	XUtil::DetourJump(OFFSET(0x247DA40, 0), &fn_247CD60);
	
	// 00 PC; 01 360; 02 PS4 ? 
	auto genFormatFiles = (BYTE)g_INI->GetInteger("CreationKit_PreCombined", "GenerateFormatFiles", 0);
	XUtil::PatchMemory(OFFSET(0x347E6E, 0), { genFormatFiles });
	XUtil::PatchMemory(OFFSET(0xDCB677, 0), { genFormatFiles });
}

