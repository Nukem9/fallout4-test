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

#include "..\..\[Common]\StdAfx.h"

// clang-format off
struct
{
	const char* BytesToFind;
	const char* BytePatch;
	int PatchSize;
	int AddressModifier;
} const Patches[] =
{
	// https://github.com/Sumwunn/AchievementsModsEnabler
	{
		"C3 40 32 FF 48 89 5C 24 40 48 89 6C 24 48",
		"\xB0\x00\xC3",
		3,
		-0x29
	},

	// https://github.com/Sumwunn/AchievementsModsEnabler
	{ 
		"C3 C6 44 24 38 00 48 8D 44 24 38 48 89 5C 24 20",
		"\xB0\x00\xC3",
		3,
		-0x28
	},
};
// clang-format on


/*
==================
Fix_PatchAchievements

Gives you the opportunity to play with mods and get achievements
==================
*/
BOOL FIXAPI Fix_PatchAchievements(VOID)
{
	// Loop through each fix and exit on the first found
	for (auto& patch : Patches)
	{
		uintptr_t addr = XUtil::FindPattern(g_CodeBase, g_CodeEnd - g_CodeBase, patch.BytesToFind);

		if (!addr)
			continue;

		XUtil::PatchMemory((addr + patch.AddressModifier), (PBYTE)patch.BytePatch, patch.PatchSize);
		return TRUE;
	}

	return FALSE;
}
